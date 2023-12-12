#include "sdkconfig.h"
#include <esp_ota_ops.h>
#include <esp_log.h>
static constexpr char TAG[] = "webserver";

#include "WebServer.hpp"
#include <cJSON.h>
#include <filesystem>

static const std::string INDEX_URI("/");
extern "C" esp_err_t INDEX(httpd_req_t *);
extern "C" esp_err_t http_redirect(httpd_req_t *req, httpd_err_code_t err);

static const std::string CATALOG_URI("api/catalog");
extern "C" esp_err_t GET_CATALOG(httpd_req_t *req);
static const std::string CATALOG_FILE_URI(CATALOG_URI + "/*");
extern "C" esp_err_t GET_CATALOG_FILE(httpd_req_t *req);
extern "C" esp_err_t PUT_CATALOG_FILE(httpd_req_t *req);
extern "C" esp_err_t DELETE_CATALOG_FILE(httpd_req_t *req);
static const std::string FIRMWARE_URI("api/firmware");
extern "C" esp_err_t GET_FIRMWARE(httpd_req_t *req);
extern "C" esp_err_t PUT_FIRMWARE(httpd_req_t *req);

// TODO increase size for efficiency (window size:5744)
static constexpr size_t CHUNK_SZ = 1048;

static std::string urlDecode(const std::string &url)
{
    std::string ret;
    for (int i = 0; i < url.length(); i++)
    {
        if (url[i] != '%')
        {
            if (url[i] == '+')
                ret += ' ';
            else
                ret += url[i];
        }
        else
        {
            int ii;
            sscanf(url.substr(i + 1, 2).c_str(), "%x", &ii);
            ret += static_cast<char>(ii);
            i = i + 2;
        }
    }
    return ret;
}

WebServer::WebServer(SneakerNet &sneakernet)
    : sneakernet(sneakernet)
{
    /*
        Turn off warnings from HTTP server as redirecting traffic will yield
        lots of invalid requests
    */
    esp_log_level_set("httpd_uri", ESP_LOG_ERROR);
    esp_log_level_set("httpd_txrx", ESP_LOG_WARN);
    esp_log_level_set("httpd_parse", ESP_LOG_ERROR);

    // Start the http server
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_open_sockets = 13;
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.stack_size = 6 * 1024; // increase stack size
    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    ESP_ERROR_CHECK(httpd_start(&handle, &config));

    // link our hooks back to our instance
    void *const self = this;
    // support index homepage (and captive portal)
    {
        httpd_uri_t hook = {
            .uri = INDEX_URI.c_str(),
            .method = HTTP_GET,
            .handler = INDEX,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
        // provide 404 redirect to support captive portal
        ESP_ERROR_CHECK(httpd_register_err_handler(handle, HTTPD_404_NOT_FOUND, http_redirect));
    }
    // CATALOG
    // support catalog listing
    {
        httpd_uri_t hook = {
            .uri = CATALOG_URI.c_str(),
            .method = HTTP_GET,
            .handler = GET_CATALOG,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }
    // support serving files
    {
        httpd_uri_t hook = {
            .uri = CATALOG_FILE_URI.c_str(),
            .method = HTTP_GET,
            .handler = GET_CATALOG_FILE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }
    // support receiving files
    {
        httpd_uri_t hook = {
            .uri = CATALOG_FILE_URI.c_str(),
            .method = HTTP_PUT,
            .handler = PUT_CATALOG_FILE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }
    // support removing files
    {
        httpd_uri_t hook = {
            .uri = CATALOG_FILE_URI.c_str(),
            .method = HTTP_DELETE,
            .handler = DELETE_CATALOG_FILE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }
    // FIRMWARE
    // support firmware check
    {
        httpd_uri_t hook = {
            .uri = FIRMWARE_URI.c_str(),
            .method = HTTP_GET,
            .handler = GET_FIRMWARE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }
    // support firmware check
    {
        httpd_uri_t hook = {
            .uri = FIRMWARE_URI.c_str(),
            .method = HTTP_PUT,
            .handler = PUT_FIRMWARE,
            .user_ctx = self,
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(handle, &hook));
    }
}

/// INDEX handler
extern "C" const char indexHtml_start[] asm("_binary_index_html_start");
extern "C" const char indexHtml_end[] asm("_binary_index_html_end");
esp_err_t INDEX(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Serving index.html");
    const size_t indexHtml_sz = indexHtml_end - indexHtml_start;
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, indexHtml_start, indexHtml_sz);
}

/// provides captive portal redirect
esp_err_t http_redirect(httpd_req_t *req, httpd_err_code_t err)
{
    // Set status
    httpd_resp_set_status(req, "302 Temporary Redirect");
    // Redirect to the "/" root directory
    httpd_resp_set_hdr(req, "Location", INDEX_URI.c_str());
    // iOS requires content in the response to detect a captive portal, simply redirecting is not sufficient.
    httpd_resp_send(req, "Redirect to the captive portal", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/// Listing of content
esp_err_t GET_CATALOG(httpd_req_t *request)
{
    ESP_LOGI(TAG, "Serving catalog");
    WebServer *const self = static_cast<WebServer *>(request->user_ctx);
    const std::vector<SneakerNet::content_t> contents = self->sneakernet.contents();
    cJSON *const items = cJSON_CreateArray();
    for (const auto &content : contents)
    {
        cJSON *const item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "filename", content.filename.c_str());
        cJSON_AddNumberToObject(item, "size", content.size);
        cJSON_AddNumberToObject(item, "timestamp", content.timestamp);
        cJSON_AddItemToArray(items, item);
    }
    const char *const response = cJSON_PrintUnformatted(items);
    ESP_LOGI(TAG, "returning \n %s", response);
    httpd_resp_set_type(request, "application/json");
    httpd_resp_send(request, response, strlen(response));
    delete response;
    cJSON_Delete(items);

    return ESP_OK;
}

static bool isValidContentName(const std::string &filename)
{
    // make sure it's only a name (no path)
    return filename.find(std::filesystem::path::preferred_separator) == filename.npos;
}

esp_err_t DELETE_CATALOG_FILE(httpd_req_t *request)
{
    WebServer *const self = static_cast<WebServer *>(request->user_ctx);
    const std::string filename = urlDecode(request->uri + CATALOG_FILE_URI.size() - sizeof('*'));
    if (isValidContentName(filename))
        self->sneakernet.removeContent(filename);
    else
        return httpd_resp_send_err(request, HTTPD_404_NOT_FOUND, nullptr);

    ESP_LOGI(TAG, "deleted file '%s'", filename.c_str());
    return ESP_OK;
}

esp_err_t GET_CATALOG_FILE(httpd_req_t *request)
{
    WebServer *const self = static_cast<WebServer *>(request->user_ctx);

    const std::string filename = urlDecode(request->uri + CATALOG_FILE_URI.size() - sizeof('*'));
    if (false == isValidContentName(filename))
        return httpd_resp_send_err(request, HTTPD_404_NOT_FOUND, nullptr);

    std::unique_ptr<char[]> buf(new char[CHUNK_SZ]);
    if (!buf)
        // FIXME httpd_err_code_t doesn't support 429 Too Many Requests
        return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try-again)");

    std::ifstream fis = self->sneakernet.readContent(filename);
    if (false == fis.is_open())
        return httpd_resp_send_err(request, HTTPD_404_NOT_FOUND, nullptr);

    httpd_resp_set_type(request, "application/octet-stream");
    esp_err_t ret = ESP_OK;
    while (true)
    {
        const size_t chunk_sz = fis.readsome(buf.get(), CHUNK_SZ);
        if (ESP_OK != httpd_resp_send_chunk(request, buf.get(), chunk_sz))
        {
            ESP_LOGW(TAG, "failed to send file");
            ret = ESP_FAIL;
            break;
        }
        if (chunk_sz == 0)
            break;
    }

    return ret;
}

esp_err_t PUT_CATALOG_FILE(httpd_req_t *request)
{
    WebServer *const self = static_cast<WebServer *>(request->user_ctx);

    const std::string filename = urlDecode(request->uri + CATALOG_FILE_URI.size() - sizeof('*'));
    if (false == isValidContentName(filename))
        return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, "illegal filename");

    // validate there is data
    if (request->content_len <= 0)
        return httpd_resp_send_err(request, HTTPD_411_LENGTH_REQUIRED, "Length required");

    // create a buffer
    std::unique_ptr<char[]> buf(new char[CHUNK_SZ]);
    if (!buf)
        // FIXME httpd_err_code_t doesn't support 429 Too Many Requests
        return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try-again)");

    const size_t file_sz = request->content_len;
    SneakerNet::InWorkContent content = self->sneakernet.newContent(filename, file_sz);
    // receive the data
    esp_err_t ret = ESP_OK;
    for (size_t remaining = request->content_len; remaining > 0;)
    {
        const int received = httpd_req_recv(request, buf.get(), MIN(remaining, CHUNK_SZ));
        if (received < 0)
        {
            ESP_LOGW(TAG, "download incomplete: %s [%d/%d]", request->uri,
                     (request->content_len - remaining), request->content_len);
            ret = ESP_FAIL;
            break;
        }
        if (false == content.write(buf.get(), received))
        {
            ret = ESP_FAIL;
            break;
        }
        remaining -= received;
    }

    if (ret == ESP_OK)
    {
        content.done();
        return ESP_OK;
    }

    return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Upload failed");
}

esp_err_t GET_FIRMWARE(httpd_req_t *request)
{
    ESP_LOGI(TAG, "Serving firmware info");
    WebServer *const self = static_cast<WebServer *>(request->user_ctx);
    cJSON *const item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "filename", "esp32-sneakernet.bin");
    cJSON_AddStringToObject(item, "version", self->sneakernet.pVersion);
    const char *const response = cJSON_PrintUnformatted(item);
    ESP_LOGI(TAG, "returning \n %s", response);
    httpd_resp_set_type(request, "application/json");
    httpd_resp_send(request, response, strlen(response));
    delete response;
    cJSON_Delete(item);
    return ESP_OK;
}

esp_err_t PUT_FIRMWARE(httpd_req_t *request)
{
    // validate there is data
    if (request->content_len <= 0)
        return httpd_resp_send_err(request, HTTPD_411_LENGTH_REQUIRED, "Length required");

    // create a buffer
    std::unique_ptr<char[]> buf(new char[CHUNK_SZ]);
    if (!buf)
        // FIXME httpd_err_code_t doesn't support 429 Too Many Requests
        return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try-again)");

    const esp_partition_t *const partition = esp_ota_get_next_update_partition(nullptr);
    esp_ota_handle_t handle;
    if (ESP_OK != esp_ota_begin(partition, request->content_len, &handle))
    {
        ESP_LOGW(TAG, "unable to begin firmware update");
        return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "unable to start ota update");
    }

    // receive the data
    esp_err_t ret = ESP_OK;
    for (size_t remaining = request->content_len; remaining > 0;)
    {
        const int received = httpd_req_recv(request, buf.get(), MIN(remaining, CHUNK_SZ));
        if (received < 0)
        {
            ESP_LOGW(TAG, "download incomplete: %s", request->uri);
            ret = ESP_FAIL;
            break;
        }
        ret = esp_ota_write(handle, buf.get(), received);
        if (ret != ESP_OK)
        {
            ESP_LOGW(TAG, "firmware update failed upon write");
            break;
        }
        remaining -= received;
    }

    if (ret != ESP_OK)
        return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "ota update failed");

    if (ESP_OK != esp_ota_end(handle))
    {
        ESP_LOGW(TAG, "unable to end firmware update");
        return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "ota end failed");
    }

    if (ESP_OK != esp_ota_set_boot_partition(partition))
    {
        ESP_LOGW(TAG, "unable to set new boot partition");
        return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, "set boot partition failed.");
    }

    // reboot
    constexpr char msg[] = "accepted update, rebooting...";
    ESP_LOGI(TAG, "%s", msg);
    httpd_resp_set_status(request, HTTPD_200);
    httpd_resp_set_type(request, HTTPD_TYPE_TEXT);
    httpd_resp_send(request, msg, sizeof(msg));
    esp_restart();

    return ESP_OK;
}