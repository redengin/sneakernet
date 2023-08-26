#include "sdkconfig.h"
#include <esp_log.h>
static constexpr char TAG[] = "adminWebserver";

#include "AdminWebServer.hpp"
#include <cJSON.h>

AdminWebServer::AdminWebServer(SneakerNet& sneakernet)
:   sneakernet(sneakernet)
{
    esp_log_level_set("httpd", ESP_LOG_DEBUG);

    // Start the https server
    httpd_ssl_config_t config = HTTPD_SSL_CONFIG_DEFAULT();
    --config.httpd.ctrl_port; // use the next control port (DEFAULT uses UINT16_MAX)
    config.httpd.max_open_sockets = 1; // only allow one admin at a time
    extern const unsigned char servercert_start[] asm("_binary_servercert_pem_start");
    extern const unsigned char servercert_end[]   asm("_binary_servercert_pem_end");
    config.servercert = servercert_start;
    config.servercert_len = servercert_end - servercert_start;
    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[]   asm("_binary_prvtkey_pem_end");
    config.prvtkey_pem = prvtkey_pem_start;
    config.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;
    config.httpd.uri_match_fn = httpd_uri_match_wildcard;
    ESP_ERROR_CHECK(httpd_ssl_start(&handle, &config));


    // TODO register admin REST methods
}
