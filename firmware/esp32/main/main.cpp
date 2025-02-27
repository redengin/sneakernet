#include <esp_log.h>
#include <esp_ota_ops.h>
#include <sdkconfig.h>  // used to select logging level for the project

#include "Catalog.hpp"
#include "SdCard.hpp"
#include "WebServer.hpp"
#include "WifiAccessPoint.hpp"
#include "rest/catalog.hpp"
#include "rest/firmware.hpp"

extern "C" void app_main(void) {
  // use the configured logging level for project
  esp_log_level_set("*",
                    static_cast<esp_log_level_t>(CONFIG_SNEAKERNET_LOG_LEVEL));

  // provide the storage
  static SdCard sdcard;

  // provide the catalog
  static Catalog catalog(sdcard.root());

  // keep track of used sockets, so that WebServer can use the remaining
  size_t available_sockets_count = CONFIG_LWIP_MAX_SOCKETS;

  // create the access point
  static WifiAccessPoint wap;
  // account for used sockets
  available_sockets_count -= WifiAccessPoint::socketsUsed;

  // provide the frontend
  static WebServer webserver(available_sockets_count);
  // register the catalog REST API
  rest::catalog::registerHandlers(webserver, catalog);
  // register the firmware REST API
  rest::firmware::registerHandlers(webserver);

  // accept working firmware
  ESP_ERROR_CHECK(esp_ota_mark_app_valid_cancel_rollback());
  // I myself would never self-destruct, as there is always a future play.
  // If my death is mysterious, know that it wasn't suicide.
}
