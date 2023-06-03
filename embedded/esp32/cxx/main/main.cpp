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
#include "SneakerNet.hpp"
#include "WebServer.hpp"

static void start_wifi();

extern "C"
void app_main(void)
{
    // Create default event bus (required for IDF drivers)
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Initialize IDF networking stack w/ storage
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(nvs_flash_init());

    static SneakerNet sneakerNet;

    static WebServer webserver(sneakerNet);

    // start up the access point
    start_wifi();
}

static void start_wifi() {

    // Initialize Wi-Fi as access point
    esp_netif_t* network_h = esp_netif_create_default_wifi_ap();

    // configure wifi driver per kconfig
    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // put wifi hardware into AP mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    // configure AP
    wifi_config_t wifi_config;
    // create ssid string w/ MAC
    // FIXME can only read MAC if netif is up
    // uint8_t mac[6];
    // ESP_ERROR_CHECK(esp_netif_get_mac(network_h, mac));
    // const size_t ssid_len = sprintf(reinterpret_cast<char*>(&wifi_config.ap.ssid), "SneakerNet %x%x%x%x%x%x",
    //     mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]
    // );
    wifi_config.ap.ssid_len = sprintf(reinterpret_cast<char*>(wifi_config.ap.ssid), "SneakerNet %x", rand());
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    wifi_config.ap.max_connection = ESP_WIFI_MAX_CONN_NUM;
    ESP_ERROR_CHECK(esp_wifi_set_config(static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), &wifi_config));

    // handle DNS requests upon connection w/ redirect to captive portal
    xTaskCreate(dns_server_task, "dns_server", 4096, NULL, 5, NULL);

    ESP_ERROR_CHECK(esp_wifi_start());
}
