#include "SneakerNet.hpp"

#include <esp_log.h>
#include <esp_event.h>

#include <sdmmc_cmd.h>
#include <esp_vfs_fat.h>
// #include <http_parser.h>
#include <filesystem>
#include <exception>

// FIXME promote to Kconfig.projbuild 
//--------------------------------------------------------------------------------
// Pin mapping
#if CONFIG_IDF_TARGET_ESP32
    #define PIN_NUM_MISO 2
    #define PIN_NUM_MOSI 15
    #define PIN_NUM_CLK  14
    #define PIN_NUM_CS   13
// #elif CONFIG_IDF_TARGET_ESP32S2
//     // adapted for internal test board ESP-32-S3-USB-OTG-Ev-BOARD_V1.0 (with ESP32-S2-MINI-1 module)
//     #define PIN_NUM_MISO 37
//     #define PIN_NUM_MOSI 35
//     #define PIN_NUM_CLK  36
//     #define PIN_NUM_CS   34
// #elif CONFIG_IDF_TARGET_ESP32C3
//     #define PIN_NUM_MISO 6
//     #define PIN_NUM_MOSI 4
//     #define PIN_NUM_CLK  5
//     #define PIN_NUM_CS   1
#endif
#if CONFIG_IDF_TARGET_ESP32S2
    #define SPI_DMA_CHAN    host.slot
#elif CONFIG_IDF_TARGET_ESP32C3
    #define SPI_DMA_CHAN    SPI_DMA_CH_AUTO
#else
    #define SPI_DMA_CHAN    1
#endif
//--------------------------------------------------------------------------------


static const char *TAG = "sneakernet";

SneakerNet::SneakerNet()
{
    state = mount_sdcard() ? State::OK : State::SDCARD_FAILED;

// FIXME 
// #ifdef CONFIG_SNEAKERNET_FILES_SUPPORT
//     std::filesystem::create_directory(FILES_PATH);
// #endif

}

bool SneakerNet::mount_sdcard()
{
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
    switch(spi_bus_initialize(static_cast<spi_host_device_t>(host.slot), &bus_cfg, SPI_DMA_CHAN))
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
        .max_files = 100,
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

const SneakerNet::Catalog SneakerNet::catalog() {
    ESP_LOGE(TAG, "catalog not implemented");
    const SneakerNet::Catalog catalog;
    return catalog;
}


std::ifstream SneakerNet::readEbook(const std::string uri)
{
    // FIXME validate uri
    return std::ifstream(uri);
}


#ifdef CONFIG_SNEAKERNET_FILES_SUPPORT
//------------------------------------------------------------------------------
const SneakerNet::FilesList SneakerNet::files()
{
    SneakerNet::FilesList ret;
    try {
        for(const auto& entry : std::filesystem::directory_iterator(FILES_PATH)) {
            if(entry.is_regular_file())
                ret.push_back(entry.path());
        }
    }
    catch(std::exception &e) {
        ESP_LOGE(TAG, "%s", e.what());
    }

    return ret;
}

std::ifstream SneakerNet::readFile(const std::string fileName)
{
    // validate uri (must not contain path separator)
    if(fileName.find(std::filesystem::path::preferred_separator) != std::string::npos)
        return std::ifstream();

    std::filesystem::path filePath(FILES_PATH);
    filePath.append(fileName);
    return std::ifstream(filePath);
}

bool SneakerNet::addFile(std::string fileName, std::istream& is, size_t fileSize)
{
    // TODO validate fileSize per SNEAKERNET_FILES_MAX_SIZE_KB

    try {
        std::filesystem::path filePath(FILES_PATH);
        filePath.append(fileName);
        std::ofstream ofs(filePath);
        ofs << is.rdbuf();
    }
    catch(std::exception &e) {
        ESP_LOGE(TAG, "%s", e.what());
        return false;
    }

    return true;
}

bool SneakerNet::deleteFile(const std::string fileName)
{
    // validate uri (must not contain path separator)
    if(fileName.find(std::filesystem::path::preferred_separator) != std::string::npos)
        return false;

    std::filesystem::path filePath(FILES_PATH);
    filePath.append(fileName);
    if(std::filesystem::remove(filePath))
        return true;
    return false;
}
//------------------------------------------------------------------------------
#endif