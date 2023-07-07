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
    // esp_netif_t* network_h = esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_ap();

    // configure wifi driver per kconfig
    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // put wifi hardware into AP mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    // configure AP
    const std::string SSID = "SneakerNet";
    wifi_config_t wifi_config = { .ap {
        .ssid = {0},        // will set later
        .password = {0},    // not used for WIFI_AUTH_OPEN
        .ssid_len = 0,      // will set later
        .channel = 0,       // will set later
        .authmode = WIFI_AUTH_OPEN,
        .ssid_hidden = 0,
        .max_connection = ESP_WIFI_MAX_CONN_NUM,
        .beacon_interval = 100,
        .pairwise_cipher = WIFI_CIPHER_TYPE_TKIP_CCMP,
        .ftm_responder = false,
        .pmf_cfg { .capable = true, .required = true }
    }};

    // create unique SSID
    // FIXME can't read MAC if netif isn't up
    // instead use rand seeded by compile time
    srand(CONFIG_APP_COMPILE_TIME_DATE);
    wifi_config.ap.ssid_len = sprintf(reinterpret_cast<char*>(&wifi_config.ap.ssid), "SneakerNet %x", rand());

    // use a random channel
    // WIFI channels https://en.wikipedia.org/wiki/List_of_WLAN_channels
    constexpr int WIFI_CHANNEL_MAX = 11;
    wifi_config.ap.channel = (rand() % WIFI_CHANNEL_MAX) + 1;

    ESP_ERROR_CHECK(esp_wifi_set_config(static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), &wifi_config));

    // handle DNS requests upon connection w/ redirect to captive portal
    xTaskCreate(dns_server_task, "dns_server", 4096, NULL, 5, NULL);

    ESP_ERROR_CHECK(esp_wifi_start());
}
