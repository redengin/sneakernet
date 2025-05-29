#include "WifiAccessPoint.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

#include <esp_wifi.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <lwip/sockets.h>
#include "dns_server.h"

WifiAccessPoint::WifiAccessPoint(const std::string &ssid,
                                 const std::string &_captivePortalUri)
  : captivePortalUri(_captivePortalUri)
{
  // tune down log chatter
  esp_log_level_set("wifi_init", ESP_LOG_WARN);

  // initialize the network stack
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  ESP_ERROR_CHECK(nvs_flash_init());

  // initialize wifi
  const wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

  // configure wifi
  wifi_config_t wifi_config;
  ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_AP, &wifi_config));
  // make it passwordless (i.e. Open)
  wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  // support multiple connections
  wifi_config.ap.max_connection = 10;
  // randomize channel
  ESP_ERROR_CHECK(esp_wifi_set_country_code(CONFIG_SNEAKERNET_WIFI_COUNTRY_CODE, false /* not used */));
  wifi_country_t country_info;
  ESP_ERROR_CHECK(esp_wifi_get_country(&country_info));
  wifi_config.ap.channel = (esp_random() % country_info.nchan) + 1; // valid channel range [1, nchan]
  // set the ssid
  ssid.copy(reinterpret_cast<char *>(wifi_config.ap.ssid), ssid.size(), 0);
  wifi_config.ap.ssid_len = ssid.size();
  // update the wifi configuration
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));

  // configure dhcp Captive-Portal Identification (https://www.rfc-editor.org/rfc/rfc8910.html)
  esp_netif_t *const netif = esp_netif_create_default_wifi_ap();
  ESP_ERROR_CHECK(esp_netif_dhcps_stop(netif));

  // magic uri to avoid login screen (doesn't appear to work)
  // static const std::string uri = "urn:ietf:params:capport:unrestricted";
  // ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVEPORTAL_URI,
  //     const_cast<char*>(uri.c_str()), uri.length()));

  // ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVEPORTAL_URI,
  //                                        const_cast<char *>(captivePortalUri.c_str()),
  //                                        captivePortalUri.length()));

  // FIXME not currently supported
  // constexpr esp_netif_dhcp_option_id_t ESP_NETIF_CAPTIVE_PORTAL_URI_IP6 =
  //     static_cast<esp_netif_dhcp_option_id_t>(103);
  // ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVE_PORTAL_URI_IP6,
  //                                        const_cast<char *>(captivePortalUri.c_str()),
  //                                        captivePortalUri.length()));

  // FIXME not currently supported
  // constexpr esp_netif_dhcp_option_id_t ESP_NETIF_CAPTIVE_PORTAL_URI_RA =
  //     static_cast<esp_netif_dhcp_option_id_t>(37);
  // ESP_ERROR_CHECK(esp_netif_dhcps_option(netif, ESP_NETIF_OP_SET, ESP_NETIF_CAPTIVE_PORTAL_URI_RA,
  //                                        const_cast<char *>(captivePortalUri.c_str()),
  //                                        captivePortalUri.length()));

  ESP_ERROR_CHECK(esp_netif_dhcps_start(netif));

  // start the DNS server that will redirect queries to captive portal
  static const dns_server_config_t config = {
    .num_of_entries = 1,
    .entries = {{.name="*", .if_key="WIFI_AP_DEF", .ip{.addr=0}}}
    // .item = {{.name="sneakernet.com", .if_key="WIFI_AP_DEF", .ip{.addr=0}}}
  };
  // DNS_SERVER_CONFIG_SINGLE("*" /* all A queries */, "WIFI_AP_DEF" /* softAP netif ID */);
  start_dns_server(&config);

  // publish the WiFi access point
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAG, "publishing SSID '%s' on channel %d",
           wifi_config.ap.ssid, wifi_config.ap.channel);
}
