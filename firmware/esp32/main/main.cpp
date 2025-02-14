#include <esp_ota_ops.h>
#include <sdkconfig.h>
#include <esp_log.h>

#include "WifiAccessPoint.hpp"
#include "WebServer.hpp"
#include "SdCard.hpp"
#include "Catalog.hpp"
#include "rest/catalog.hpp"


extern "C"
void app_main(void)
{
    // allow debug logging
    esp_log_level_set("*", static_cast<esp_log_level_t>(CONFIG_FREE_LIBRARY_LOG_LEVEL));

    size_t available_sockets_count = CONFIG_LWIP_MAX_SOCKETS;

    // create the access point
    static WifiAccessPoint wap("Free Library");
    // account for DNS socket
    --available_sockets_count;

    // provide the frontend
    available_sockets_count -= 3;   // account for HTTP server sockets
    static WebServer webserver(available_sockets_count);

    // provide the storage
    static SdCard sdcard;

    // provide the catalog
    static Catalog catalog(sdcard.root());
    // register the catalog REST API
    rest::catalog::registerHandlers(webserver, catalog);

    // accept working firmware
    ESP_ERROR_CHECK(esp_ota_mark_app_valid_cancel_rollback());
}
