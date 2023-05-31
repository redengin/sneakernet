#include "SneakerNet.hpp"

#include <esp_log.h>
#include <esp_event.h>

#include <sdmmc_cmd.h>
#include <http_parser.h>

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
    if(! mount_sdcard())
        state = State::SDCARD_FAILED;

    // TODO check for password

    state = State::OK;
}

bool SneakerNet::mount_sdcard()
{
    ESP_LOGI(TAG, "Initializing SD card");
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
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
        .max_transfer_sz = 4000,
        .flags = 0, /* UNSUPPORTED */
        .intr_flags = 0, /* UNSUPPORTED */
    };
    esp_err_t ret;
    ret = spi_bus_initialize(static_cast<spi_host_device_t>(host.slot), &bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return false;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = static_cast<gpio_num_t>(PIN_NUM_CS);
    slot_config.host_id = static_cast<spi_host_device_t>(host.slot);

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount("", &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK)
        return false;

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    return false;
}

const SneakerNet::Catalog SneakerNet::catalog() {
    ESP_LOGE(TAG, "catalog not implemented");
    const SneakerNet::Catalog catalog;
    return catalog;
}


std::ifstream SneakerNet::readEbook(const std::string uri)
{
    return std::ifstream(uri);
}
