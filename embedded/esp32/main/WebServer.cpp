#include "sdkconfig.h"
#include <esp_log.h>
static constexpr char TAG[] = "webserver";

#include "WebServer.hpp"
#include <cJSON.h>

static const std::string INDEX_URI("/");
extern "C" esp_err_t INDEX(httpd_req_t*);
extern "C" esp_err_t http_redirect(httpd_req_t *req, httpd_err_code_t err);
static const std::string CATALOG_URI(INDEX_URI + "catalog");
extern "C" esp_err_t CATALOG(httpd_req_t* req);
static const std::string CATALOG_FILE_URI(CATALOG_URI + "/*");
extern "C" esp_err_t GET_CATALOG_FILE(httpd_req_t* req);
extern "C" esp_err_t PUT_CATALOG_FILE(httpd_req_t* req);
static constexpr size_t CHUNK_SZ = 1048;

WebServer::WebServer(SneakerNet& sneakernet)
:   sneakernet(sneakernet)
{
    // FIXME debug use only
    // esp_log_level_set(TAG, ESP_LOG_DEBUG);

    /*
        Turn of warnings from HTTP server as redirecting traffic will yield
        lots of invalid requests
    */
    esp_log_level_set("httpd_uri", ESP_LOG_ERROR);
    esp_log_level_set("httpd_txrx", ESP_LOG_WARN);
    esp_log_level_set("httpd_parse", ESP_LOG_ERROR);

    // Start the httpd server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.stack_size = 6*1024;
    config.uri_match_fn = httpd_uri_match_wildcard;
    // TODO max_open_sockets increase (LWIP_SOCKETS - 3)
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    ESP_ERROR_CHECK(httpd_start(&handle, &config));

    // link our hooks back to our instance
    void* const self = this;
    // support index homepage (and captive portal)
    {   httpd_uri_t hook = {
            .uri = INDEX_URI.c_str(),
            .method = HTTP_GET,
            .handler = INDEX,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
        // provide 404 redirect to support captive portal
        ESP_ERROR_CHECK(httpd_register_err_handler(handle, HTTPD_404_NOT_FOUND, http_redirect));
    }

    // support catalog listing
    {   httpd_uri_t hook = {
            .uri = CATALOG_URI.c_str(),
            .method = HTTP_GET,
            .handler = CATALOG,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }

    // support serving files
    {   httpd_uri_t hook = {
            .uri = CATALOG_FILE_URI.c_str(),
            .method = HTTP_GET,
            .handler = GET_CATALOG_FILE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }

    // support receiving files
    {   httpd_uri_t hook = {
            .uri = CATALOG_FILE_URI.c_str(),
            .method = HTTP_PUT,
            .handler = PUT_CATALOG_FILE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }
}

/// INDEX handler
extern "C" const char indexHtml_start[] asm("_binary_index_html_start");
extern "C" const char indexHtml_end[] asm("_binary_index_html_end");
esp_err_t INDEX(httpd_req_t* req)
{
    ESP_LOGI(TAG, "Serving index.html");
    const size_t indexHtml_sz = indexHtml_end - indexHtml_start;
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, indexHtml_start, indexHtml_sz);
}

/// provides captive portal redirect
esp_err_t http_redirect(httpd_req_t *req, httpd_err_code_t err)
{
    ESP_LOGI(TAG, "Redirecting to root");
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" root directory
    httpd_resp_set_hdr(req, "Location", INDEX_URI.c_str());
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


/// Listing of content
esp_err_t CATALOG(httpd_req_t* request)
{
    ESP_LOGI(TAG, "Serving catalog");
    WebServer* const self = static_cast<WebServer*>(request->user_ctx);
    const std::map<Catalog::filename_t, Catalog::Entry> catalog = self->sneakernet.catalog.items();
    cJSON* const items = cJSON_CreateArray();
    for(const auto& [filename, entry] : catalog) {
        cJSON* const catalogItem = cJSON_CreateObject();
        cJSON_AddStringToObject(catalogItem, "filename", filename.c_str());
        cJSON_AddStringToObject(catalogItem, "sha256", entry.sha256.c_str());
    }
    httpd_resp_set_type(request, "application/json");
    const char* const response = cJSON_PrintUnformatted(items);
    httpd_resp_send(request, response, strlen(response));
    delete response;
    cJSON_Delete(items);

    return ESP_OK;
}

esp_err_t GET_CATALOG_FILE(httpd_req_t* request)
{
    char* buf = new char[CHUNK_SZ];
    if(buf == NULL) {
        // FIXME httpd_err_code_t doesn't support 429 Too Many Requests
        return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try-again)"); 
    }

    WebServer* const self = static_cast<WebServer*>(request->user_ctx);
    const std::string filename = request->uri + CATALOG_FILE_URI.size() - sizeof("*");
    std::ifstream fis = self->sneakernet.catalog.readItem(filename);
    if(fis.bad())
        return httpd_resp_send_err(request, HTTPD_404_NOT_FOUND, NULL);

    httpd_resp_set_type(request, "application/octet-stream");
    esp_err_t ret = ESP_OK;
    while(true) {
        const size_t chunk_sz = fis.readsome(buf, CHUNK_SZ);
        esp_err_t status = httpd_resp_send_chunk(request, buf, chunk_sz);
        if(status != ESP_OK) {
            ESP_LOGW(TAG, "failed to send file [esp_err=%d]", status);
            ret = ESP_FAIL;
            break;
        }
        if(chunk_sz == 0) break;
    }
    delete buf;

    return ret;
}

esp_err_t PUT_CATALOG_FILE(httpd_req_t* request) {
    // validate there is data
    if(request->content_len <= 0) {
        return httpd_resp_send_err(request, HTTPD_411_LENGTH_REQUIRED, "Length required"); 
    }

    // create a buffer
    char* buf = new char[CHUNK_SZ];
    if(buf == NULL) {
        // FIXME httpd_err_code_t doesn't support 429 Too Many Requests
        return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try-again)"); 
    }

    WebServer* const self = static_cast<WebServer*>(request->user_ctx);
    const std::string filename = request->uri + CATALOG_FILE_URI.size() - sizeof("*");
    const size_t file_sz = request->content_len;
    Catalog::InWorkItem item = self->sneakernet.catalog.newItem(filename, file_sz);
    if(false == item.ofs.is_open())
        return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, "Illegal path"); 

    // receive the data
    esp_err_t ret = ESP_OK;
    for(size_t remaining = request->content_len; remaining > 0;) {
        const int received = httpd_req_recv(request, buf, MIN(remaining, CHUNK_SZ));
        if(received < 0) {
            ESP_LOGW(TAG, "download incomplete: %s", request->uri);
            ret = ESP_FAIL;
            break;
        }
        item.ofs.write(buf, received);
        remaining -= received;
    }
    delete buf;

    if(ret != ESP_OK)
        return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Item not received"); 

    if(item.add())
        ESP_LOGI(TAG, "added item %s", request->uri);
    else
        return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, item.getMessages().c_str()); 

    return ESP_OK;
}
