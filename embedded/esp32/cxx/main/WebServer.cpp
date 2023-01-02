#include "WebServer.hpp"

#include <esp_log.h>
#include <esp_event.h>

static const char *TAG = "www";

extern "C"
esp_err_t SneakerNet_Get(httpd_req_t*);

WebServer::WebServer() {
    /*
        Turn of warnings from HTTP server as redirecting traffic will yield
        lots of invalid requests
    */
    esp_log_level_set("httpd_uri", ESP_LOG_ERROR);
    esp_log_level_set("httpd_txrx", ESP_LOG_ERROR);
    esp_log_level_set("httpd_parse", ESP_LOG_ERROR);

    // Start the httpd server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    // TODO max_open_sockets increase (LWIP_SOCKETS - 3)
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    ESP_ERROR_CHECK(httpd_start(&handle, &config));

    // support GET
    httpd_uri_t GET_hook;
    GET_hook.method = HTTP_GET;
    GET_hook.uri = "*";
    GET_hook.handler = SneakerNet_Get;
    httpd_register_uri_handler(handle, &GET_hook);
}

/// GET handler
extern "C" const char root_start[] asm("_binary_index_html_start");
extern "C" const char root_end[] asm("_binary_index_html_end");
esp_err_t SneakerNet_Get(httpd_req_t* req) {
    const uint32_t root_len = root_end - root_start;

    ESP_LOGI(TAG, "Serve root");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, root_start, root_len);

    return ESP_OK;
}