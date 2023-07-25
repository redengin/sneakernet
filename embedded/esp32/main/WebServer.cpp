#include "sdkconfig.h"
#include <esp_log.h>
static constexpr char TAG[] = "webserver";

#include "WebServer.hpp"
#include <cJSON.h>

extern "C" esp_err_t INDEX(httpd_req_t*);
extern "C" esp_err_t http_redirect(httpd_req_t *req, httpd_err_code_t err);
extern "C" esp_err_t CATALOG(httpd_req_t* req);
extern "C" esp_err_t GET_CATALOG_FILE(httpd_req_t* req);
extern "C" esp_err_t PUT_CATALOG_FILE(httpd_req_t* req);

static constexpr size_t CHUNK_SZ = 1048;
class HttpInputStreamBuf : public std::streambuf
{
public:
    explicit HttpInputStreamBuf(httpd_req_t* const _req)
    : req(_req)
    {}

protected:
    char chunk[CHUNK_SZ];
    int underflow() override
    {
        ESP_LOGI(TAG, "adding chunk to streambuf");
        const size_t chunk_sz = httpd_req_recv(req, chunk, sizeof(chunk));
        if(chunk_sz <= 0) {
            ESP_LOGI(TAG, "chunk stream ended chunk_sz: %d", chunk_sz);
            return traits_type::eof();
        }
        ESP_LOGI(TAG, "added chunk_sz: %d chunk to stream", chunk_sz);

        setg(chunk, chunk, (chunk+chunk_sz));
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
            .uri = "/",
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
    httpd_resp_set_hdr(req, "Location", "/");
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


/// Listing of content
esp_err_t CATALOG(httpd_req_t* req)
{
    ESP_LOGI(TAG, "Serving catalog");
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

esp_err_t GET_CATALOG_FILE(httpd_req_t* req)
{
    char* buf = new char[CHUNK_SZ];
    if(buf == NULL) {
        // FIXME httpd_err_code_t doesn't support 429 Too Many Requests
        return httpd_resp_send_err(req, HTTPD_408_REQ_TIMEOUT, "Too many requests (try-again)"); 
    }

    WebServer* const self = static_cast<WebServer*>(req->user_ctx);
    std::string filename = req->uri;
    std::ifstream fis = self->sneakerNet.readCatalogItem(filename);
    if(fis.bad())
        return httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, NULL);

    httpd_resp_set_type(req, "application/octet-stream");
    esp_err_t ret = ESP_OK;
    while(true) {
        const size_t chunk_sz = fis.readsome(buf, CHUNK_SZ);
        esp_err_t status = httpd_resp_send_chunk(req, buf, chunk_sz);
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

esp_err_t PUT_CATALOG_FILE(httpd_req_t* req) {
    // validate there is data
    if(req->content_len <= 0) {
        return httpd_resp_send_err(req, HTTPD_411_LENGTH_REQUIRED, "Length required"); 
    }

    // create a buffer
    char* buf = new char[CHUNK_SZ];
    if(buf == NULL) {
        // FIXME httpd_err_code_t doesn't support 429 Too Many Requests
        return httpd_resp_send_err(req, HTTPD_408_REQ_TIMEOUT, "Too many requests (try-again)"); 
    }

    // create a new catalog item
    WebServer* const self = static_cast<WebServer*>(req->user_ctx);
    SneakerNet::NewItem item = self->sneakerNet.createNewCatalogItem(req->uri, req->content_len);
    // is the uri valid?
    if(item.isBad()) {
        return httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Illegal path"); 
    }
    // can we get an output stream?
    std::ofstream ofs = item.getOfstream();
    if(!ofs.is_open()) {
        ESP_LOGW(TAG, "PUT_CATALOG_FILE, unable to open filestream");
        // FIXME httpd_err_code_t doesn't support 429 Too Many Requests
        return httpd_resp_send_err(req, HTTPD_408_REQ_TIMEOUT, "Too many requests (try-again)"); 
    }

    // receive the data
    esp_err_t ret = ESP_OK;
    for(size_t remaining = req->content_len; remaining > 0;) {
        int received = httpd_req_recv(req, buf, MIN(remaining, CHUNK_SZ));
        if(received < 0) {
            ESP_LOGW(TAG, "download incomplete: %s", req->uri);
            ret = ESP_FAIL;
            break;
        }
        ofs.write(buf, received);
        remaining -= received;
    }
    ofs.close();
    delete buf;

    if(ret != ESP_OK)
        return httpd_resp_send_err(req, HTTPD_408_REQ_TIMEOUT, "Item not received"); 

    // add the item to the catalog
    switch(self->sneakerNet.addNewCatalogItem(item))
    {
        case SneakerNet::AddNewCatalogItemStatus::OK : {
            ESP_LOGI(TAG, "item accepted: %s", req->uri);
            break;
        }
        // TODO return error strings for rejection reason
        default: {
            ESP_LOGW(TAG, "item rejected: %s", req->uri);
            return httpd_resp_send_err(req, HTTPD_408_REQ_TIMEOUT, "Item rejected"); 
        }
    }

    return ESP_OK;
}
