#include "WebServer.hpp"

#include <esp_log.h>
#include <esp_event.h>

#include <cJSON.h>
#include "sdkconfig.h"

static const char *TAG = "www";

extern "C" esp_err_t INDEX(httpd_req_t*);
extern "C" esp_err_t http_error_handler(httpd_req_t *req, httpd_err_code_t err);
extern "C" esp_err_t CATALOG(httpd_req_t* req);
extern "C" esp_err_t GET_EBOOK(httpd_req_t* req);
extern "C" esp_err_t PUT_EBOOK(httpd_req_t* req);
#ifdef CONFIG_SNEAKERNET_FILES_SUPPORT
    extern "C" esp_err_t LIST_FILES(httpd_req_t* req);
    extern "C" esp_err_t GET_FILE(httpd_req_t* req);
    extern "C" esp_err_t PUT_FILE(httpd_req_t* req);
    extern "C" esp_err_t DELETE_FILE(httpd_req_t* req);
#endif


const size_t CHUNK_SZ = 1048;


//TODO remove need for `self` by using httpd_req_t.handle or httpd_req_t.user_ctx
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
    {   httpd_uri_t hook;
        hook.method = HTTP_GET;
        hook.uri = "/";
        hook.handler = INDEX;
        httpd_register_uri_handler(handle, &hook);
        httpd_register_err_handler(handle, HTTPD_404_NOT_FOUND, http_error_handler);
    }

    // support catalog listing
    {   httpd_uri_t hook;
        hook.method = HTTP_GET;
        hook.uri = "/catalog";
        hook.handler = CATALOG;
        httpd_register_uri_handler(handle, &hook);
    }

    // support ebook download
    {   httpd_uri_t hook;
        hook.method = HTTP_GET;
        hook.uri = "/ebook/*";
        hook.handler = GET_EBOOK;
        httpd_register_uri_handler(handle, &hook);
    }

    // support ebook upload
    {   httpd_uri_t hook;
        hook.method = HTTP_PUT;
        hook.uri = "/ebook/*";
        hook.handler = PUT_EBOOK;
        httpd_register_uri_handler(handle, &hook);
    }

#ifdef CONFIG_SNEAKERNET_FILES_SUPPORT
    // support files listing
    {   httpd_uri_t hook;
        hook.method = HTTP_GET;
        hook.uri = "/files";
        hook.handler = LIST_FILES;
        httpd_register_uri_handler(handle, &hook);
    }

    // support file download
    {   httpd_uri_t hook;
        hook.method = HTTP_GET;
        hook.uri = "/files/*";
        hook.handler = GET_FILE;
        httpd_register_uri_handler(handle, &hook);
    }

    // support file upload
    {   httpd_uri_t hook;
        hook.method = HTTP_PUT;
        hook.uri = "/files/*";
        hook.handler = PUT_FILE;
        httpd_register_uri_handler(handle, &hook);
    }

    // support file delete
    {   httpd_uri_t hook;
        hook.method = HTTP_DELETE;
        hook.uri = "/files/*";
        hook.handler = DELETE_FILE;
        httpd_register_uri_handler(handle, &hook);
    }
#endif
}

/// INDEX handler
extern "C" const char indexHtml_start[] asm("_binary_index_html_start");
extern "C" const char indexHtml_end[] asm("_binary_index_html_end");
esp_err_t INDEX(httpd_req_t* req)
{
    const size_t indexHtml_sz = indexHtml_end - indexHtml_start;
    ESP_LOGI(TAG, "Serving index.html");
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, indexHtml_start, indexHtml_sz);
}

esp_err_t http_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    ESP_LOGI(TAG, "upon http client failure, sending index");
    return httpd_resp_send_err(req, err, indexHtml_start);
}


/// Listing of content JSON [Publisher UID][Content UID]
/// @param req 
/// @return 
esp_err_t CATALOG(httpd_req_t* req)
{
    SneakerNet::Catalog catalog = self->sneakerNet.catalog();
    cJSON* const catalog_object = cJSON_CreateObject();
    for(const auto &pair : catalog) {
        cJSON* const contentUuids = cJSON_CreateArray();
        for(const auto &contentUuid : pair.second) {
            cJSON* const item = cJSON_CreateString(contentUuid.c_str());
            cJSON_AddItemToArray(contentUuids, item);
        }
        cJSON_AddItemToObject(catalog_object, pair.first.c_str(), contentUuids);
    }
    const char* const ret = cJSON_PrintUnformatted(catalog_object);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, ret, strlen(ret));
    delete ret;
    cJSON_Delete(catalog_object);
    return ESP_OK;
}

esp_err_t GET_EBOOK(httpd_req_t* req)
{
    std::ifstream ebook_is = self->sneakerNet.readEbook(req->uri);
    if(ebook_is.bad())
        return http_error_handler(req, HTTPD_404_NOT_FOUND);

    httpd_resp_set_type(req, "application/epub+zip");
    char chunk[CHUNK_SZ];
    while(true) {
        const size_t chunk_sz = ebook_is.readsome(chunk, sizeof(chunk));
        esp_err_t status = httpd_resp_send_chunk(req, chunk, chunk_sz);
        if(status != ESP_OK) {
            ESP_LOGW(TAG, "failed to send ebook [esp_err=%d]", status);
            return status;
        }
        if(chunk_sz == 0) break;
    }
    return ESP_OK;
}

esp_err_t PUT_EBOOK(httpd_req_t* req) {
    // TODO
    return ESP_FAIL;
}



#ifdef CONFIG_SNEAKERNET_FILES_SUPPORT
//-------------------------------------------------------------------------------
extern "C" esp_err_t LIST_FILES(httpd_req_t* req)
{
    SneakerNet::FilesList files = self->sneakerNet.files();
    cJSON* const files_array = cJSON_CreateArray();
    for(const auto &file : files) {
        cJSON* const item = cJSON_CreateString(file.c_str());
        cJSON_AddItemToArray(files_array, item);
    }
    const char* const ret = cJSON_PrintUnformatted(files_array);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, ret, strlen(ret));
    delete ret;
    cJSON_Delete(files_array);
    return ESP_OK;
}

extern "C" esp_err_t GET_FILE(httpd_req_t* req)
{
    // TODO
    return ESP_FAIL;
}

extern "C" esp_err_t PUT_FILE(httpd_req_t* req) 
{
    // TODO
    return ESP_FAIL;
}

extern "C" esp_err_t DELETE_FILE(httpd_req_t* req)
{
    // TODO
    return ESP_FAIL;
}
//-------------------------------------------------------------------------------
#endif
