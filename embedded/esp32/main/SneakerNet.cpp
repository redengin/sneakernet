#include "SneakerNet.hpp"

#include <esp_event.h>
#include <sdmmc_cmd.h>
#include <esp_vfs_fat.h>
// #include <http_parser.h>
#include <filesystem>
#include <string.h>
#include <exception>
#include <esp_log.h>
static const char *TAG = "sneakernet";

// Pin mapping - avoiding JTAG pins (12, 13, 14, 15)
//--------------------------------------------------------------------------------
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 4
#define PIN_NUM_CLK  5
#define PIN_NUM_CS   18
//--------------------------------------------------------------------------------
const std::string SneakerNet::MOUNT_PATH = "/sdcard";
const std::string SneakerNet::CATALOG_DIR = "/catalog";

SneakerNet::SneakerNet()
    : catalog(MOUNT_PATH + CATALOG_DIR)
{
    // mount the sd card
    state = mount_sdcard() ? State::OK : State::SDCARD_FAILED;

    // initialize the catalog
    if(state != State::SDCARD_FAILED) {
        if(std::filesystem::exists(catalog.path))
            catalog.init();
        else {
            // ensure sdcard catalog directory
            if(false == std::filesystem::create_directory(catalog.path))
                ESP_LOGE(TAG, "failed to create catalog directory");
        }
    }
}

bool SneakerNet::mount_sdcard()
{
    esp_log_level_set("vfs_fat_sdmmc", ESP_LOG_DEBUG);
    ESP_LOGI(TAG, "Initializing sdcard");
    const sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    constexpr int UNUSED = -1;
    const spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = UNUSED,
        .quadhd_io_num = UNUSED,
        .data4_io_num = UNUSED,
        .data5_io_num = UNUSED,
        .data6_io_num = UNUSED,
        .data7_io_num = UNUSED,
        .max_transfer_sz = 0,   // use full dma buffer support
        .flags = 0, /* not used */
        .intr_flags = 0, /* not used */
    };
    switch(spi_bus_initialize(static_cast<spi_host_device_t>(host.slot), &bus_cfg, SDSPI_DEFAULT_DMA))
    {
        case ESP_OK: {
            ESP_LOGI(TAG, "sdcard spi connection created");
            break;
        }
        default: {
            ESP_LOGE(TAG, "failed to initialized sdcard spi connection");
            return false;
        }
    }

    ESP_LOGI(TAG, "Mounting filesystem");
    // no support for hardware CARD_DETECT or WRITE_PROTECT
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = static_cast<gpio_num_t>(PIN_NUM_CS);
    slot_config.host_id = static_cast<spi_host_device_t>(host.slot);
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 10,
        .allocation_unit_size = 200 * 1024,
        .disk_status_check_enable = false
    };
    switch(esp_vfs_fat_sdspi_mount(MOUNT_PATH.c_str(), &host, &slot_config, &mount_config, &card))
    {
        case ESP_OK: {
            ESP_LOGI(TAG, "sdcard mounted");
            sdmmc_card_print_info(stdout, card);
            return true;
        }
        default: {
            ESP_LOGE(TAG, "failed to mount sdcard");
            return false;
        }
    }
}

bool SneakerNet::isValidContentPath(const std::string& path) const {
    return (std::string::npos == path.find(CATALOG_DIR.size()));
}

std::ifstream SneakerNet::readCatalogItem(const std::string& path)
{
    // validate path
    if(!SneakerNet::isValidContentPath(path)) {
        // invalid path return disconnected stream
        ESP_LOGI(TAG, "readCatalogItem: invalid path '%s'", path.c_str());
        return std::ifstream();
    }

    return std::ifstream(MOUNT_PATH + path, std::ios_base::in | std::ios_base::binary);
}


const std::string SneakerNet::CATALOG_NEW_ITEM_SUFFIX = ".inwork";
const std::filesystem::path SneakerNet::NewItem::getInworkPath() const {
    return std::filesystem::path(MOUNT_PATH)/CATALOG_DIR/(filename + CATALOG_NEW_ITEM_SUFFIX);
}

std::ofstream SneakerNet::NewItem::getOfstream() {
    if(isBad())
        return std::ofstream();

    ESP_LOGI(TAG, "NewItem::getOfstream path:'%s'", getInworkPath().c_str());
    return std::ofstream(getInworkPath(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
}

SneakerNet::NewItem SneakerNet::createNewCatalogItem(const std::string& path, const size_t size) {
    // validate path
    if(!SneakerNet::isValidContentPath(path)) {
        // invalid path return disconnected stream
        ESP_LOGI(TAG, "addCatalogItem: invalid path '%s'", path.c_str());
        return SneakerNet::NewItem();
    }

    // TODO clean up oldest files to make room for new item

    return SneakerNet::NewItem(std::filesystem::path(path).filename());
}

SneakerNet::AddNewCatalogItemStatus SneakerNet::addNewCatalogItem(const NewItem& item) {

    SneakerNet::AddNewCatalogItemStatus status = validateNewCatalogItem(item);

    std::error_code err;
    switch(status) {
        case OK: {
            std::filesystem::path actualPath = item.getInworkPath();
            actualPath.replace_filename(item.filename);

            // remove file about to be replaced
            if(std::filesystem::exists(actualPath))
                if(false == std::filesystem::remove(actualPath, err))
                    ESP_LOGE(TAG, "failed to remove overwritten entry [error: %s]", err.message().c_str());

            // rename without the inwork suffix
            std::filesystem::rename(item.getInworkPath(), actualPath, err);
            if(err) {
                ESP_LOGE(TAG, "failed to rename %s to %s [error: %s]", item.getInworkPath().c_str(), actualPath.c_str(), err.message().c_str());
                // fail and fallthrough to default
                status = FAILED;
                [[fallthrough]];
            }
            else break;    
        }
        default: {
            // delete the inwork item
            if(false == std::filesystem::remove(item.getInworkPath(), err))
                ESP_LOGE(TAG, "failed to remove %s [error: %s]", item.getInworkPath().c_str(), err.message().c_str());
        }
    }

    if(status == OK)
        if(false == catalog.add(item.filename))
            status = FAILED;

    return status;    
}

SneakerNet::AddNewCatalogItemStatus SneakerNet::validateNewCatalogItem(const NewItem&) {

    // TODO validate per librarian preferences

    return OK;
}
