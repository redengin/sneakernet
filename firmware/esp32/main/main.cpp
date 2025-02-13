#include <esp_ota_ops.h>

#include <sdkconfig.h>
#include "WifiAccessPoint.hpp"
#include "WebServer.hpp"
#include "SdCard.hpp"
#include "Catalog.hpp"
#include "rest/catalog.hpp"

extern "C"
void app_main(void)
{
    size_t available_sockets_count = CONFIG_LWIP_MAX_SOCKETS;

    // create the access point
    static WifiAccessPoint wap;
    // account for used sockets
    available_sockets_count -= wap.socketsUsed;

    // provide the frontend
    static WebServer webserver(available_sockets_count);

    // provide the storage
    static SdCard sdcard;

    // provide the catalog
    static Catalog catalog(sdcard.root());
    // register the catalog REST API
    rest::catalog::registerHandlers(webserver, catalog);


    // accept working firmware
    esp_ota_mark_app_valid_cancel_rollback();
}
