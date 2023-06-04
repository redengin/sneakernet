#include "sdkconfig.h"
#include <esp_log.h>
static constexpr char TAG[] = "webserver";

#include "WebServer.hpp"
#include <cJSON.h>

extern "C" esp_err_t INDEX(httpd_req_t*);
extern "C" esp_err_t http_redirect(httpd_req_t *req, httpd_err_code_t err);
extern "C" esp_err_t CATALOG(httpd_req_t* req);
extern "C" esp_err_t GET_EBOOK(httpd_req_t* req);
extern "C" esp_err_t PUT_EBOOK(httpd_req_t* req);
#ifdef CONFIG_SNEAKERNET_FILES_SUPPORT
    extern "C" esp_err_t LIST_FILES(httpd_req_t* req);
    extern "C" esp_err_t GET_FILE(httpd_req_t* req);
    extern "C" esp_err_t PUT_FILE(httpd_req_t* req);
    extern "C" esp_err_t DELETE_FILE(httpd_req_t* req);
#endif
static constexpr size_t CHUNK_SZ = 1048;
static esp_err_t send_file(httpd_req_t* const req, std::ifstream& fis);
class HttpInputStreamBuf : public std::streambuf
{
public:
    explicit HttpInputStreamBuf(httpd_req_t* const _req)
    : req(_req)
    {}

protected:
    int underflow() override
    {
        char chunk[CHUNK_SZ];
        const size_t chunk_sz = httpd_req_recv(req, chunk, sizeof(chunk));
        if(chunk_sz == 0)
            return traits_type::eof();

        const std::streamsize accepted_sz = xsputn(chunk, chunk_sz);
        assert(accepted_sz == chunk_sz);

        return chunk[0];
    }

private:
    httpd_req_t* const req;
};



WebServer::WebServer(SneakerNet& _sneakerNet)
:   sneakerNet(_sneakerNet)
{
    // FIXME debug use only
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

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

    // link our hooks back to our instance
    void* const self = this;
    // support index homepage
    {   httpd_uri_t hook = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = INDEX,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
        ESP_ERROR_CHECK(httpd_register_err_handler(handle, HTTPD_404_NOT_FOUND, http_redirect));
    }

    // support catalog listing
    {   httpd_uri_t hook = {
            .uri = "/catalog",
            .method = HTTP_GET,
            .handler = CATALOG,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }

    // support ebook download
    {   httpd_uri_t hook = {
            .uri = "/ebook/*",
            .method = HTTP_GET,
            .handler = GET_EBOOK,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }

    // support ebook upload
    {   httpd_uri_t hook = {
            .uri = "/ebook/*",
            .method = HTTP_PUT,
            .handler = PUT_EBOOK,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }

#ifdef CONFIG_SNEAKERNET_FILES_SUPPORT
    ESP_LOGI(TAG, "Adding FILES support");
    // support files listing
    {   httpd_uri_t hook = {
            .uri = FILES_URI.c_str(),
            .method = HTTP_GET,
            .handler = LIST_FILES,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }

    // support file download
    {   httpd_uri_t hook = {
            .uri = FILE_URI.c_str(),
            .method = HTTP_GET,
            .handler = GET_FILE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }

    // support file upload
    {   httpd_uri_t hook = {
            .uri = FILE_URI.c_str(),
            .method = HTTP_PUT,
            .handler = PUT_FILE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }

    // support file delete
    {   httpd_uri_t hook = {
            .uri = (FILE_URI).c_str(),
            .method = HTTP_DELETE,
            .handler = DELETE_FILE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }
#endif
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

esp_err_t http_redirect(httpd_req_t *req, httpd_err_code_t err)
{
    ESP_LOGI(TAG, "Redirecting to root");
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" root directory
    httpd_resp_set_hdr(req, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


/// Listing of content JSON [Publisher UID][Content UID]
/// @param req 
/// @return 
esp_err_t CATALOG(httpd_req_t* req)
{
    ESP_LOGI(TAG, "Serving ebook catalog");
    WebServer* const self = static_cast<WebServer*>(req->user_ctx);
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
    WebServer* const self = static_cast<WebServer*>(req->user_ctx);
    std::ifstream fis = self->sneakerNet.readEbook(req->uri);
    if(fis.bad())
        return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "");

    httpd_resp_set_type(req, "application/epub+zip");
    return send_file(req, fis);
}

esp_err_t PUT_EBOOK(httpd_req_t* req) {
    // WebServer* const self = static_cast<WebServer*>(req->user_ctx);
    // TODO
    return ESP_FAIL;
}



#ifdef CONFIG_SNEAKERNET_FILES_SUPPORT
//-------------------------------------------------------------------------------
extern "C" esp_err_t LIST_FILES(httpd_req_t* req)
{
    ESP_LOGI(TAG, "Serving file list");
    WebServer* const self = static_cast<WebServer*>(req->user_ctx);
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
    ESP_LOGI(TAG, "Sending file");
    WebServer* const self = static_cast<WebServer*>(req->user_ctx);
    const char* const fileName = req->uri + self->FILES_URI.length() + sizeof('/');
    std::ifstream fis = self->sneakerNet.readFile(fileName);
    if(fis.bad())
        return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "");

    httpd_resp_set_type(req, HTTPD_TYPE_OCTET);
    return send_file(req, fis);
}

extern "C" esp_err_t PUT_FILE(httpd_req_t* req) 
{
    ESP_LOGI(TAG, "Receiving file");
    WebServer* const self = static_cast<WebServer*>(req->user_ctx);
    const char* const fileName = req->uri + self->FILES_URI.length() + sizeof('/');
    HttpInputStreamBuf hsb(req);
    std::istream his(&hsb);
    if(self->sneakerNet.addFile(fileName, his, req->content_len))
        return ESP_OK;
    return ESP_FAIL;
}

extern "C" esp_err_t DELETE_FILE(httpd_req_t* req)
{
    ESP_LOGI(TAG, "Deleting file");
    WebServer* const self = static_cast<WebServer*>(req->user_ctx);
    const char* const fileName = req->uri + self->FILES_URI.length() + sizeof('/');
    if(self->sneakerNet.deleteFile(fileName))
        return ESP_OK;
    return ESP_FAIL;
}
//-------------------------------------------------------------------------------
#endif


static esp_err_t send_file(httpd_req_t* const req, std::ifstream& fis)
{
    char chunk[CHUNK_SZ];
    while(true) {
        const size_t chunk_sz = fis.readsome(chunk, sizeof(chunk));
        esp_err_t status = httpd_resp_send_chunk(req, chunk, chunk_sz);
        if(status != ESP_OK) {
            ESP_LOGW(TAG, "failed to send file [esp_err=%d]", status);
            return status;
        }
        if(chunk_sz == 0) break;
    }
    return ESP_OK;
}
