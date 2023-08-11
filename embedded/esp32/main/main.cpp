#include "dns_server.h"
#include "SneakerNet.hpp"
#include "WebServer.hpp"
#include "AdminWebServer.hpp"

#include <nvs_flash.h>
#include <esp_wifi.h>
#include <hal/efuse_hal.h>
static void start_wifi_ap(void);


extern "C"
void app_main(void)
{
    // create default event bus (required for IDF drivers)
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // create sneakernet controller
    static SneakerNet sneakerNet;

    // start the network
    ESP_ERROR_CHECK(esp_netif_init());

    // create http interface to sneakernet catalog
    static WebServer webserver(sneakerNet);

    // FIXME fails to allocate socket
    // // create https interface for sneakernet admin
    // static AdminWebServer adminWebserver(sneakerNet);

    // create the wifi access point captive portal
    start_wifi_ap();

    // sneaker net working, don't roll back firmware
    esp_ota_mark_app_valid_cancel_rollback();
}

/// creates a wifi access point captive portal
static void start_wifi_ap(void)
{
    // IDF wifi drivers requires access to non-volatile flash
    ESP_ERROR_CHECK(nvs_flash_init());

    // configure netif as wifi access point
    esp_netif_create_default_wifi_ap();

    // configure wifi driver per kconfig
    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // put wifi hardware into access point mode
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    // configure access point
    wifi_config_t wifi_config = { .ap {
        .ssid = {0},        // will set below
        .password = {0},    // not used for WIFI_AUTH_OPEN
        .ssid_len = 0,      // will set below
        .channel = 0,       // will set below
        .authmode = WIFI_AUTH_OPEN,
        .ssid_hidden = 0,
        .max_connection = ESP_WIFI_MAX_CONN_NUM,
        .beacon_interval = 100,
        .pairwise_cipher = WIFI_CIPHER_TYPE_TKIP_CCMP,
        .ftm_responder = false,
        .pmf_cfg { .capable = true, .required = true }
    }};

    // get the hard-coded mac address
    uint8_t mac[6];
    efuse_hal_get_mac(mac);
    // set wifi_config.ap.ssid per the mac address
    wifi_config.ap.ssid_len = snprintf(reinterpret_cast<char*>(&wifi_config.ap.ssid),
                                      sizeof(wifi_config.ap.ssid),
                                      "SneakerNet %X%X%X%X%X%X",
                                      mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

    // set a "random" channel based on mac random
    srand(*reinterpret_cast<int*>(mac + 2));
    // WIFI channels https://en.wikipedia.org/wiki/List_of_WLAN_channels
    constexpr int WIFI_CHANNEL_MAX = 11;    // North America limit
    wifi_config.ap.channel = (rand() % WIFI_CHANNEL_MAX) + 1;

    ESP_ERROR_CHECK(esp_wifi_set_config(static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), &wifi_config));

    // handle DNS requests upon connection w/ redirect to captive portal
    ESP_ERROR_CHECK(false == xTaskCreate(dns_server_task, "dns_server", 4096, NULL, 5, NULL));

    ESP_ERROR_CHECK(esp_wifi_start());
}
