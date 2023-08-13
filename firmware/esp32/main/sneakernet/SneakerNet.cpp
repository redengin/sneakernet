#include "SneakerNet.hpp"

#include <esp_app_desc.h>
#include <sdmmc_cmd.h>
#include <esp_log.h>
static const char *TAG = "sneakernet";
#include <filesystem>

constexpr char MOUNT_DIR[] = "/sdcard";
constexpr char CATALOG_DIR[] = "catalog";

SneakerNet::SneakerNet()
    : catalog(std::filesystem::path(MOUNT_DIR)/CATALOG_DIR)
{
    // initialize the firmwareSha256
    // const int sz = esp_app_get_elf_sha256(firmwareSha256, 2 * sizeof(firmwareSha256));
    // if(sz != sizeof(firmwareSha256))
    //     ESP_LOGW(TAG, "mismatched sha256 length [is:%d, expected:%d]", sz, sizeof(firmwareSha256));
    // ESP_LOGI(TAG, "firmwware SHA256 %s", firmwareSha256);
    const esp_app_desc_t* const pDesc = esp_app_get_description();
    pVersion = pDesc->version;

    // mount the sd card
    mount_sdcard();

    // initialize the catalog
    catalog.init();
}

void SneakerNet::mount_sdcard()
{
// Pin mapping - avoiding JTAG pins (12, 13, 14, 15)
//--------------------------------------------------------------------------------
#define PIN_NUM_CS   2
#define PIN_NUM_MOSI 4
#define PIN_NUM_CLK  5
#define PIN_NUM_MISO 18
//--------------------------------------------------------------------------------
    ESP_LOGI(TAG, "Mounting sdcard filesystem");

    esp_log_level_set("vfs_fat_sdmmc", ESP_LOG_ERROR);
    const sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    constexpr int UNUSED = 0;
    constexpr int UNUSED_IO = -1;
    const spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = UNUSED_IO,
        .quadhd_io_num = UNUSED_IO,
        .data4_io_num = UNUSED_IO,
        .data5_io_num = UNUSED_IO,
        .data6_io_num = UNUSED_IO,
        .data7_io_num = UNUSED_IO,
        .max_transfer_sz = 0,   // 0: use full dma buffer support
        .flags = UNUSED,
        .intr_flags = UNUSED,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(static_cast<spi_host_device_t>(host.slot),
                                       &bus_cfg, SDSPI_DEFAULT_DMA));

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = static_cast<gpio_num_t>(PIN_NUM_CS);
    slot_config.host_id = static_cast<spi_host_device_t>(host.slot);
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 10,    // FIXME tune per RAM availability
        .allocation_unit_size = 512,    // required value for sdcards for wear leveling
        .disk_status_check_enable = false,
    };
    ESP_ERROR_CHECK(esp_vfs_fat_sdspi_mount(MOUNT_DIR,
                                            &host, &slot_config, &mount_config, &pCard));

    sdmmc_card_print_info(stdout, pCard);
}



// bool SneakerNet::isValidContentPath(const std::string& path) const {
//     // FIXME
//     // return (std::string::npos == path.find(CATALOG_DIR.size()));
//     return false;
// }


// const std::string SneakerNet::CATALOG_NEW_ITEM_SUFFIX = ".inwork";
// const std::filesystem::path SneakerNet::NewItem::getInworkPath() const {
//     return std::filesystem::path("/")/MOUNT_DIR/CATALOG_DIR/(filename + CATALOG_NEW_ITEM_SUFFIX);
// }

// std::ofstream SneakerNet::NewItem::getOfstream() {
//     if(isBad())
//         return std::ofstream();

//     ESP_LOGI(TAG, "NewItem::getOfstream path:'%s'", getInworkPath().c_str());
//     return std::ofstream(getInworkPath(), std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
// }

// SneakerNet::NewItem SneakerNet::createNewCatalogItem(const std::string& path, const size_t size) {
//     // validate path
//     if(!SneakerNet::isValidContentPath(path)) {
//         // invalid path return disconnected stream
//         ESP_LOGI(TAG, "addCatalogItem: invalid path '%s'", path.c_str());
//         return SneakerNet::NewItem();
//     }

//     // TODO clean up oldest files to make room for new item

//     return SneakerNet::NewItem(std::filesystem::path(path).filename());
// }

// SneakerNet::AddNewCatalogItemStatus SneakerNet::addNewCatalogItem(const NewItem& item) {

//     SneakerNet::AddNewCatalogItemStatus status = validateNewCatalogItem(item);

//     std::error_code err;
//     switch(status) {
//         case OK: {
//             std::filesystem::path actualPath = item.getInworkPath();
//             actualPath.replace_filename(item.filename);

//             // remove file about to be replaced
//             if(std::filesystem::exists(actualPath))
//                 if(false == std::filesystem::remove(actualPath, err))
//                     ESP_LOGE(TAG, "failed to remove overwritten entry [error: %s]", err.message().c_str());

//             // rename without the inwork suffix
//             std::filesystem::rename(item.getInworkPath(), actualPath, err);
//             if(err) {
//                 ESP_LOGE(TAG, "failed to rename %s to %s [error: %s]", item.getInworkPath().c_str(), actualPath.c_str(), err.message().c_str());
//                 // fail and fallthrough to default
//                 status = FAILED;
//                 [[fallthrough]];
//             }
//             else break;    
//         }
//         default: {
//             // delete the inwork item
//             if(false == std::filesystem::remove(item.getInworkPath(), err))
//                 ESP_LOGE(TAG, "failed to remove %s [error: %s]", item.getInworkPath().c_str(), err.message().c_str());
//         }
//     }

//     if(status == OK)
//         if(false == catalog.add(item.filename))
//             status = FAILED;

//     return status;    
// }

// SneakerNet::AddNewCatalogItemStatus SneakerNet::validateNewCatalogItem(const NewItem&) {

//     // TODO validate per librarian preferences

//     return OK;
// }
