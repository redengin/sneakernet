#include "WifiAccessPoint.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

#include <esp_wifi.h>
#include <freertos/task.h>
#include <hal/efuse_hal.h>
#include <nvs_flash.h>
#include <lwip/sockets.h>
#include "dns_server.h"

WifiAccessPoint::WifiAccessPoint() {
  // tune down log chatter
  esp_log_level_set("wifi_init", ESP_LOG_WARN);

  // initialize the network stack
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(nvs_flash_init());
  esp_netif_t* const netif = esp_netif_create_default_wifi_ap();

  // initialize wifi
  wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

  // configure wifi
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  wifi_config_t wifi_config;
  ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_AP, &wifi_config));
  // make it passwordless
  wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  // set the ssid via tha MAC
  uint8_t mac[6];
  efuse_hal_get_mac(mac);
  wifi_config.ap.ssid_len =
      snprintf(reinterpret_cast<char *>(&wifi_config.ap.ssid), sizeof(wifi_config.ap.ssid),
               "SneakerNet %X%X%X%X%X%X",
               mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  // randomize channel
    srand(*reinterpret_cast<uint32_t*>(mac + 2));
    wifi_config.ap.channel = (rand() % (CONFIG_SNEAKERNET_WIFI_CHANNEL_MAX + 1));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

  // add captive portal option to dhcps
  static const std::string captiveportal_uri = "http://192.168.4.1/CAPTIVE_PORTAL";
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_stop(netif));
  ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVEPORTAL_URI,
                                         const_cast<char*>(captiveportal_uri.c_str()),
                                         captiveportal_uri.length()));
  ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_start(netif));


  // Start the DNS server that will redirect all queries to the softAP IP
  dns_server_config_t config = DNS_SERVER_CONFIG_SINGLE("*" /* all A queries */, "WIFI_AP_DEF" /* softAP netif ID */);
  start_dns_server(&config);
  
  // publish the WiFi access point
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAG, "publishing SSID '%s'", wifi_config.ap.ssid);
}
