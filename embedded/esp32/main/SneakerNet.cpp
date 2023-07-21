#include "SneakerNet.hpp"

#include <esp_event.h>
#include <sdmmc_cmd.h>
#include <esp_vfs_fat.h>
// #include <http_parser.h>
#include <filesystem>
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

SneakerNet::SneakerNet()
{
    state = mount_sdcard() ? State::OK : State::SDCARD_FAILED;
}

const std::string SneakerNet::MOUNT_PATH = "/sdcard";
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

const std::string SneakerNet::CATALOG_DIR = "/catalog";
const SneakerNet::Catalog SneakerNet::catalog() {
    ESP_LOGE(TAG, "catalog not implemented");
    const SneakerNet::Catalog catalog;
    return catalog;
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

    return std::ifstream(MOUNT_PATH + "/" + path);
}


const std::string SneakerNet::CATALOG_NEW_ITEM_SUFFIX = ".inwork";
std::string SneakerNet::NewItem::getInworkPath() const {
    return (MOUNT_PATH + "/" + path + CATALOG_NEW_ITEM_SUFFIX);
}

std::ofstream SneakerNet::NewItem::getOfstream() {
    if(isBad())
        return std::ofstream();

    ESP_LOGI(TAG, "NewItem::getOfstream path:'%s'", getInworkPath().c_str());
    return std::ofstream(getInworkPath());
}

SneakerNet::NewItem SneakerNet::createNewCatalogItem(const std::string& path, const size_t size) {
    // validate path
    if(!SneakerNet::isValidContentPath(path)) {
        // invalid path return disconnected stream
        ESP_LOGI(TAG, "addCatalogItem: invalid path '%s'", path.c_str());
        return SneakerNet::NewItem();
    }

    // TODO clean up oldest files to make room for new item

    return SneakerNet::NewItem(path);
}


SneakerNet::AddNewCatalogItemStatus SneakerNet::addNewCatalogItem(const NewItem& item) {

    SneakerNet::AddNewCatalogItemStatus status = validateNewCatalogItem(item);
    switch(status) {
        case OK: {
            // remove the suffix
            const std::string inwork = item.getInworkPath();
            const std::string actual = inwork.substr(0, (inwork.size() - CATALOG_NEW_ITEM_SUFFIX.size()));
            rename(inwork.c_str(), actual.c_str());

            // TODO add the item to the catalog
            break;
        }
        default: {
            // delete the inwork item
            remove(item.getInworkPath().c_str());
        }
    }
    return status;    
}

SneakerNet::AddNewCatalogItemStatus SneakerNet::validateNewCatalogItem(const NewItem&) {

    // TODO validate per librarian preferences

    return OK;
}
