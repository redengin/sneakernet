// extra profiling debug
// #define SNEAKERNET_CHARACTERIZE_SDCARD


#include "SdCard.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

#include <driver/sdmmc_host.h>
#include <sdmmc_cmd.h>
#include <esp_vfs_fat.h>
#include <soc/spi_pins.h>

SdCard::SdCard() {
  // tune down log chatter
  esp_log_level_set("sdspi_transaction", ESP_LOG_WARN);

  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  // specialize host config
  host.max_freq_khz = SDMMC_FREQ_26M;   // max rate currently possible under non-IOMUX SPI
//   host.max_freq_khz = 80000;   // max rate currently possible under IOMUX SPI
  const spi_bus_config_t spi_bus_config{
      // pins configured for straight-thru to mmc peripheral
      .mosi_io_num = GPIO_NUM_15,
      .miso_io_num = GPIO_NUM_4,
      .sclk_io_num = GPIO_NUM_2,
    // HSPI IOMUX pins (but not per function, so doesn't appear to work)
    //   .mosi_io_num = GPIO_NUM_13,
    //   .miso_io_num = GPIO_NUM_14,
    //   .sclk_io_num = GPIO_NUM_12,
      .quadwp_io_num = GPIO_NUM_NC, // not used
      .quadhd_io_num = GPIO_NUM_NC, // not used
      .data4_io_num = GPIO_NUM_NC,  // not used
      .data5_io_num = GPIO_NUM_NC,  // not used
      .data6_io_num = GPIO_NUM_NC,  // not used
      .data7_io_num = GPIO_NUM_NC,  // not used
      .data_io_default_level = false,
      .max_transfer_sz = CONFIG_FATFS_VFS_FSTAT_BLKSIZE,
      .flags = 0,                   // not used
      .isr_cpu_id = ESP_INTR_CPU_AFFINITY_1,    // put SPI on CPU 1
      .intr_flags = 0 /* not used */
  };
  ESP_ERROR_CHECK(spi_bus_initialize(static_cast<spi_host_device_t>(host.slot),
                                     &spi_bus_config, SPI_DMA_CH_AUTO));


  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 10,                                  // TODO maximize to support users
      .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,    /* reformat parameter */
      .disk_status_check_enable = false,                /* ignore physical sd card detection */
      .use_one_fat = true                               /* reformat parameter */
  };
  sdmmc_card_t* card;
  ESP_ERROR_CHECK(esp_vfs_fat_sdspi_mount(path, &host, &slot_config, &mount_config, &card));

  // show sdcard info
  sdmmc_card_print_info(stdout, card);

#ifdef SNEAKERNET_CHARACTERIZE_SDCARD
  characterize();
#endif
}

#ifdef SNEAKERNET_CHARACTERIZE_SDCARD
#include <filesystem>
#include <fstream>
static void characterize_write(const std::filesystem::path &filepath);
static void characterize_read(const std::filesystem::path &filepath);
static void report_rate(const size_t sz, const size_t seconds);

void SdCard::characterize() const {
    const std::filesystem::path filepath = root() / ".ratetest";
    characterize_write(filepath);
    characterize_read(filepath);
    std::filesystem::remove(filepath);
}

void report_rate(const size_t sz, const size_t seconds)
{
    constexpr double BYTES_PER_MEGABYTE = 1 << 20;
    const double mbps = (sz / BYTES_PER_MEGABYTE) / seconds;
    ESP_LOGI(SdCard::TAG, "rate %f mbps", mbps);
}

constexpr size_t CHUNK_SIZE = CONFIG_FATFS_VFS_FSTAT_BLKSIZE;
constexpr size_t CHUNKS = 1000;
static char buffer[CHUNK_SIZE];

void characterize_write(const std::filesystem::path &filepath)
{
    ESP_LOGI(SdCard::TAG, "characterizing write rate");

    std::ofstream file(filepath, std::ios::binary);
    auto start_time = std::chrono::high_resolution_clock::now();
    for(size_t chunks=0; chunks<CHUNKS; ++chunks)
        file.write(buffer, CHUNK_SIZE);
    auto end_time = std::chrono::high_resolution_clock::now();

    const size_t total_bytes = CHUNK_SIZE * CHUNKS;
    const std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    report_rate(total_bytes, elapsed_seconds.count());
}

void characterize_read(const std::filesystem::path &filepath)
{
    ESP_LOGI(SdCard::TAG, "characterizing read rate");

    std::ifstream file(filepath, std::ios::binary);
    auto start_time = std::chrono::high_resolution_clock::now();
    for(size_t chunks=0; chunks<CHUNKS; ++chunks)
        file.read(buffer, CHUNK_SIZE);
    auto end_time = std::chrono::high_resolution_clock::now();

    const size_t total_bytes = CHUNK_SIZE * CHUNKS;
    const std::chrono::duration<double> elapsed_seconds = end_time - start_time;
    report_rate(total_bytes, elapsed_seconds.count());
}
#endif
