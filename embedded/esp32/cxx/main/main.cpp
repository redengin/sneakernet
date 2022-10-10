#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_log.h>
#include <sdkconfig.h>

#include <nvs_flash.h>
#include <esp_netif.h>
#include <esp_wifi.h>
#include <lwip/inet.h>
#include <cstring>
#include "dns_server.h"
#include "WebServer.hpp"

static const char *TAG = "sneakernet";

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
// #define BLINK_GPIO CONFIG_BLINK_GPIO

static void initialize_wifi();

extern "C"
void app_main(void)
{
    // Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    // Initialize wifi
    initialize_wifi();

    static WebServer webserver;
    (void)webserver;
}

/* wifi initialization */
/* ---------------------------------------------------------------------- */
static void initialize_wifi() {
    // Initialize networking stack
    ESP_ERROR_CHECK(esp_netif_init());

    // Initialize Wi-Fi including netif with default config
    esp_netif_create_default_wifi_ap();

    // Initialise ESP32 in SoftAP mode
    // TODO create SSID with MAC
    #define EXAMPLE_ESP_WIFI_SSID "Sneakernet"

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_config_t wifi_config = {};
    wifi_config.ap.ssid_len = strlen(EXAMPLE_ESP_WIFI_SSID);
    memcpy(wifi_config.ap.ssid, EXAMPLE_ESP_WIFI_SSID, wifi_config.ap.ssid_len);
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    wifi_config.ap.max_connection = 5;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    xTaskCreate(dns_server_task, "dns_server", 4096, NULL, 5, NULL);
}
