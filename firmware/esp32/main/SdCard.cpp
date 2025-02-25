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

  // const sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  const sdmmc_host_t host = {
      /* workaround for incomplete SDPSI_HOST_DEFAULT() */
      .flags = SDMMC_HOST_FLAG_SPI | SDMMC_HOST_FLAG_DEINIT_ARG,
      .slot = HSPI_HOST,
      .max_freq_khz = SDMMC_FREQ_DEFAULT,
      .io_voltage = 3.3f,
      .init = &sdspi_host_init,
      .set_bus_width = nullptr,
      .get_bus_width = nullptr,
      .set_bus_ddr_mode = nullptr,
      .set_card_clk = &sdspi_host_set_card_clk,
      .set_cclk_always_on = nullptr,
      .do_transaction = &sdspi_host_do_transaction,
      .deinit_p = &sdspi_host_remove_device,
      .io_int_enable = &sdspi_host_io_int_enable,
      .io_int_wait = &sdspi_host_io_int_wait,
      .command_timeout_ms = 0, /* use default */
      .get_real_freq = &sdspi_host_get_real_freq,
      .input_delay_phase = SDMMC_DELAY_PHASE_0,
      .set_input_delay = nullptr,
      .dma_aligned_buffer = nullptr,
      .pwr_ctrl_handle = nullptr,
      .get_dma_info = &sdspi_host_get_dma_info};
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
      .max_transfer_sz = 0,         /* use default */
      .flags = 0,                   /* not used */
      .isr_cpu_id = ESP_INTR_CPU_AFFINITY_0,
      .intr_flags = 0 /* not used */
  };
  ESP_ERROR_CHECK(spi_bus_initialize(static_cast<spi_host_device_t>(host.slot),
                                     &spi_bus_config, SPI_DMA_CH_AUTO));

  const sdspi_device_config_t slot_config = {
      .host_id = SDSPI_DEFAULT_HOST,
      .gpio_cs = SDSPI_SLOT_NO_CS,
      .gpio_cd = SDSPI_SLOT_NO_CD,
      .gpio_wp = SDSPI_SLOT_NO_WP,
      .gpio_int = SDSPI_SLOT_NO_INT,
      .gpio_wp_polarity = false /* not used */
  };
  const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 10,                               // FIXME
      .allocation_unit_size = CONFIG_WL_SECTOR_SIZE, /* reformat parameter */
      .disk_status_check_enable = false,
      .use_one_fat = true /* reformat parameter */
  };
  sdmmc_card_t* card;
  auto result =
      esp_vfs_fat_sdspi_mount(path, &host, &slot_config, &mount_config, &card);
  ESP_LOGI(TAG, "mount sdcard result [%x %s]", result, esp_err_to_name(result));
  if (ESP_OK == result) sdmmc_card_print_info(stdout, card);
}