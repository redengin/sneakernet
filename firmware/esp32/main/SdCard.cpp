#include "SdCard.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

#include <esp_vfs_fat.h>
#include <sdmmc_cmd.h>

SdCard::SdCard() {
  // tune down log chatter
  esp_log_level_set("sdspi_transaction", ESP_LOG_WARN);

  // initialize the SPI bus
  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  host.slot = HSPI_HOST;
  const spi_bus_config_t spi_bus_config{
      // pins configured for straight-thru to mmc peripheral
      .mosi_io_num = GPIO_NUM_15,
      .miso_io_num = GPIO_NUM_4,
      .sclk_io_num = GPIO_NUM_2,
      .quadwp_io_num = GPIO_NUM_NC, /* not used */
      .quadhd_io_num = GPIO_NUM_NC, /* not used */
      .data4_io_num = GPIO_NUM_NC,  /* not used */
      .data5_io_num = GPIO_NUM_NC,  /* not used */
      .data6_io_num = GPIO_NUM_NC,  /* not used */
      .data7_io_num = GPIO_NUM_NC,  /* not used */
      .data_io_default_level = 0,   /* not used */
      .max_transfer_sz = 0,         /* use default */
      .flags = 0,                   /* not used */
      .isr_cpu_id = ESP_INTR_CPU_AFFINITY_0,
      .intr_flags = 0 /* not used */
  };
  ESP_ERROR_CHECK(spi_bus_initialize(static_cast<spi_host_device_t>(host.slot),
                                     &spi_bus_config, SPI_DMA_CH_AUTO));

  // mount the SD card
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs = SDSPI_SLOT_NO_CS;
  const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 10,                                // FIXME
      .allocation_unit_size = CONFIG_WL_SECTOR_SIZE,  /* reformat parameter */
      .disk_status_check_enable = false,
      .use_one_fat = true                             /* reformat parameter */
  };
  sdmmc_card_t* card;
  auto result =
      esp_vfs_fat_sdspi_mount(path, &host, &slot_config, &mount_config, &card);
  ESP_LOGI(TAG, "mount sdcard result [%x %s]", result, esp_err_to_name(result));
  if (ESP_OK == result) sdmmc_card_print_info(stdout, card);
}