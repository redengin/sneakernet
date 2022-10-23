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

/* Use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
// #define BLINK_GPIO CONFIG_BLINK_GPIO

static void initialize_wifi();

extern "C"
void app_main(void)
{
    // Initialize NVS (required for IDF drivers)
    ESP_ERROR_CHECK(nvs_flash_init());

    // Create default event bus (required for IDF drivers)
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize IDF networking stack
    ESP_ERROR_CHECK(esp_netif_init());

    initialize_wifi();

    static WebServer webserver;

    // start up the access point
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void initialize_wifi() {
    // Initialize Wi-Fi as access point
    esp_netif_create_default_wifi_ap();

    // configure wifi driver per kconfig
    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // put wifi hardware into AP mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    // configure AP
    wifi_config_t wifi_config;
    // create ssid string w/ MAC
    uint8_t mac[6];
    ESP_ERROR_CHECK(esp_base_mac_addr_get(mac));
    const size_t ssid_len = sprintf(reinterpret_cast<char*>(&wifi_config.ap.ssid), "SneakerNet %x%x%x%x%x%x",
        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
    );
    wifi_config.ap.ssid_len = ssid_len;
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    wifi_config.ap.max_connection = ESP_WIFI_MAX_CONN_NUM;
    ESP_ERROR_CHECK(esp_wifi_set_config(static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), &wifi_config));

    // handle DNS requests upon connection w/ redirect to captive portal
    xTaskCreate(dns_server_task, "dns_server", 4096, NULL, 5, NULL);
}
