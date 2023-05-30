#include "WebServer.hpp"

#include <esp_log.h>
#include <esp_event.h>

#include <cJSON.h>

static const char *TAG = "www";

extern "C" esp_err_t INDEX(httpd_req_t*);
extern "C" esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err);
extern "C" esp_err_t CATALOG_LIST(httpd_req_t* req);
//TODO remove need for `self` by using httpd_req_t.handle
static WebServer* self;

WebServer::WebServer(SneakerNet& _sneakerNet) {
    self = this;
    sneakerNet = _sneakerNet;
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

    // support index homepage
    httpd_uri_t INDEX_hook;
    INDEX_hook.method = HTTP_GET;
    INDEX_hook.uri = "/";
    INDEX_hook.handler = INDEX;
    httpd_register_uri_handler(handle, &INDEX_hook);
    httpd_register_err_handler(handle, HTTPD_404_NOT_FOUND, http_404_error_handler);

    // support catalog listing
    httpd_uri_t CATALOG_LIST_hook;
    CATALOG_LIST_hook.method = HTTP_GET;
    CATALOG_LIST_hook.uri = "/catalog";
    CATALOG_LIST_hook.handler = CATALOG_LIST;
    httpd_register_uri_handler(handle, &CATALOG_LIST_hook);
}

/// INDEX handler
extern "C" const char indexHtml_start[] asm("_binary_index_html_start");
extern "C" const char indexHtml_end[] asm("_binary_index_html_end");
esp_err_t INDEX(httpd_req_t* req)
{
    const size_t indexHtml_sz = indexHtml_end - indexHtml_start;
    ESP_LOGI(TAG, "Serving index.html");
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, indexHtml_start, indexHtml_sz);
    return ESP_OK;
}

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    ESP_LOGI(TAG, "upon 404, sending index");
    return INDEX(req);
}

/// Listing of content JSON [Publisher UID][Content UID]
/// @param req 
/// @return 
esp_err_t CATALOG_LIST(httpd_req_t* req)
{
    SneakerNet::Catalog catalog = self->sneakerNet.catalog();
    httpd_resp_set_type(req, "application/json");




    ESP_LOGE(TAG, "not implemented");
    return ESP_OK;
}

