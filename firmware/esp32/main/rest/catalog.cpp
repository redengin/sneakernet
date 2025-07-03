#include "catalog.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL
using rest::catalog::TAG;

#include <cJSON.h>
#include "utils.hpp"

struct Context
{
    WebServer &webserver;
    Catalog &catalog;
};

extern "C" esp_err_t sync_handler(httpd_req_t *);
extern "C" esp_err_t content_handler(httpd_req_t *);
extern "C" esp_err_t title_handler(httpd_req_t *);
extern "C" esp_err_t icon_handler(httpd_req_t *);

void rest::catalog::registerHandlers(WebServer &webserver, Catalog &catalog)
{
    // create the context object
    static Context context{
        .webserver = webserver,
        .catalog = catalog,
    };

    // register the handlers with the webserver
    webserver.registerUriHandler(
        httpd_uri_t{.uri = uri_sync.begin(),
                    .method = static_cast<http_method>(HTTP_GET),
                    .handler = sync_handler,
                    .user_ctx = &context});
    webserver.registerUriHandler(
        httpd_uri_t{.uri = uri_content_wildcard.begin(),
                    .method = static_cast<http_method>(HTTP_ANY),
                    .handler = content_handler,
                    .user_ctx = &context});
    webserver.registerUriHandler(
        httpd_uri_t{.uri = uri_title_wildcard.begin(),
                    .method = static_cast<http_method>(HTTP_ANY),
                    .handler = title_handler,
                    .user_ctx = &context});
    webserver.registerUriHandler(
        httpd_uri_t{.uri = uri_icon_wildcard.begin(),
                    .method = static_cast<http_method>(HTTP_ANY),
                    .handler = icon_handler,
                    .user_ctx = &context});
}

esp_err_t sync_handler(httpd_req_t *request)
{
    auto context = reinterpret_cast<Context *>(request->user_ctx);
    auto response = request;

    auto folderInfo = context->catalog.getFiles();

    // turn the folderInfo into JSON
    auto response_json = cJSON_CreateObject();
    for (auto &entry : folderInfo.value())
    {
        auto e = cJSON_CreateObject();
        cJSON_AddBoolToObject(e, "isFolder", false);
        cJSON_AddNumberToObject(e, "size", entry.fileInfo.size);
        cJSON_AddStringToObject(
            e, "timestamp", rest::timestamp(entry.fileInfo.timestamp).c_str());
        cJSON_AddBoolToObject(e, "hasIcon", entry.hasIcon);
        if (entry.fileInfo.title.has_value())
            cJSON_AddStringToObject(e, "title",
                                    entry.fileInfo.title.value().c_str());

        cJSON_AddItemToObject(response_json, entry.name.c_str(), e);
    }
    char *const data = cJSON_PrintUnformatted(response_json);
    cJSON_Delete(response_json);

    // handle out of memory condition
    if (data == nullptr)
        return httpd_resp_send_custom_err(response, rest::TOO_MANY_REQUESTS, nullptr);

    // return the data
    httpd_resp_set_type(response, "application/json");
    ESP_LOGD(rest::catalog::TAG, "sync json [%s]", data);
    auto ret = httpd_resp_send(response, data, strlen(data));
    // cleanup json allocations
    cJSON_free(data);
    return ret;
}

static std::string catalogPath(const char *const uri, const std::string_view uri_wildcard)
{
    auto path = std::string(uri + uri_wildcard.length() - sizeof('*'));

    // remove redundant initial '/' (due to openapi required path folder parameter)
    if (path.starts_with('/'))
        path.erase(0, 1);

    // remove any query
    auto pos = path.find('?');
    if (pos != std::string::npos)
        path.erase(pos);

    // remove any fragment
    pos = path.find('#');
    if (pos != std::string::npos)
        path.erase(pos);

    // decode the http tokens in-place
    rest::httpDecode(path);

    return path;
}

static esp_err_t GET_FOLDER(httpd_req_t *const request);
static esp_err_t PUT_FILE(httpd_req_t *const request);
esp_err_t content_handler(httpd_req_t *request)
{
    auto context = reinterpret_cast<Context *>(request->user_ctx);
    auto response = request;

    const auto path = catalogPath(request->uri, rest::catalog::uri_content_wildcard);
    if (path.empty() || path.ends_with('/'))
    {
        // handle folder operations
        switch (request->method)
        {
        case HTTP_GET:
        {
            ESP_LOGD(rest::catalog::TAG, "handling GET FOLDER for '%s'", path.c_str());
            return GET_FOLDER(request);
        }
        case HTTP_PUT:
        {
            ESP_LOGD(rest::catalog::TAG, "handling PUT FOLDER for '%s'", path.c_str());
            if (context->catalog.makeFolder(path))
                return httpd_resp_send(response, nullptr, 0);
            else
                return httpd_resp_send_err(response, HTTPD_500_INTERNAL_SERVER_ERROR, nullptr);
        }
        case HTTP_DELETE:
        {
            ESP_LOGD(rest::catalog::TAG, "handling DELETE FOLDER for '%s'", path.c_str());
            if (!context->catalog.hasFolder(path))
                return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);

            if (context->catalog.removeFolder(path))
                return httpd_resp_send(response, nullptr, 0);
            else
                return httpd_resp_send_err(response, HTTPD_500_INTERNAL_SERVER_ERROR, nullptr);
        }
        default:
            return rest::ILLEGAL_REQUEST(request);
        }
    }
    else
    {
        // handle file operations
        switch (request->method)
        {
        case HTTP_GET:
        {
            ESP_LOGD(rest::catalog::TAG, "handling GET FILE for %s", path.c_str());

            auto data = context->catalog.readContent(path);
            if (data.has_value())
            {
                rest::sendOctetStream(request, data.value());
                // sendOctetStream has set the response status
                return httpd_resp_send(request, nullptr, 0);
            }
            else
                return httpd_resp_send_err(request, HTTPD_404_NOT_FOUND, nullptr);
        }
        case HTTP_PUT:
        {
            ESP_LOGD(rest::catalog::TAG, "handling PUT FILE for %s", path.c_str());

            return PUT_FILE(request);
        }
        case HTTP_DELETE:
        {
            ESP_LOGD(rest::catalog::TAG, "handling DELETE FILE for %s", path.c_str());

            if (!context->catalog.hasFile(path))
                return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);

            if (context->catalog.removeFile(path))
                return httpd_resp_send(response, nullptr, 0);
            else
                return httpd_resp_send_err(response, HTTPD_500_INTERNAL_SERVER_ERROR, nullptr);
        }
        default:
            return rest::ILLEGAL_REQUEST(request);
        }
    }
}

static esp_err_t GET_FOLDER(httpd_req_t *const request)
{
    auto context = reinterpret_cast<Context *>(request->user_ctx);
    auto response = request;

    const auto path = catalogPath(request->uri, rest::catalog::uri_content_wildcard);
    auto folderInfo = context->catalog.getFolder(path);
    if (!folderInfo.has_value())
        return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);

    // turn the folderInfo into JSON
    auto response_json = cJSON_CreateObject();
    for (auto &entry : folderInfo.value())
    {
        auto e = cJSON_CreateObject();
        if (entry.isFolder)
        {
            cJSON_AddBoolToObject(e, "isFolder", true);
            cJSON_AddBoolToObject(e, "hasIcon", entry.hasIcon);
        }
        else
        {
            cJSON_AddBoolToObject(e, "isFolder", false);
            cJSON_AddNumberToObject(e, "size", entry.fileInfo.size);
            cJSON_AddStringToObject(
                e, "timestamp", rest::timestamp(entry.fileInfo.timestamp).c_str());
            cJSON_AddBoolToObject(e, "hasIcon", entry.hasIcon);
            if (entry.fileInfo.title.has_value())
                cJSON_AddStringToObject(e, "title",
                                        entry.fileInfo.title.value().c_str());
        }
        cJSON_AddItemToObject(response_json, entry.name.c_str(), e);
    }
    char *const data = cJSON_PrintUnformatted(response_json);
    cJSON_Delete(response_json);

    // handle out of memory condition
    if (data == nullptr)
        return httpd_resp_send_custom_err(response, rest::TOO_MANY_REQUESTS, nullptr);

    // return the data
    httpd_resp_set_type(response, "application/json");
    ESP_LOGD(rest::catalog::TAG, "folder json [%s]", data);
    auto ret = httpd_resp_send(response, data, strlen(data));
    // cleanup json allocations
    cJSON_free(data);
    return ret;
}

static esp_err_t PUT_FILE(httpd_req_t *const request)
{
    auto context = reinterpret_cast<Context *>(request->user_ctx);
    auto response = request;

    // validate http header Content-Length
    if (!request->content_len)
    {
        return httpd_resp_send_err(response, HTTPD_411_LENGTH_REQUIRED, nullptr);
    }
    // validate http header X-Timestamp
    char buffer[rest::catalog::ISO8601_MAX_LENGTH];
    if (ESP_OK != httpd_req_get_hdr_value_str(response,
                                              rest::catalog::XFILETIMESTAMP,
                                              buffer, sizeof(buffer)))
    {
        return httpd_resp_send_err(response, HTTPD_400_BAD_REQUEST,
                                   "X-Timestamp header required");
    }
    auto timestamp = rest::timestamp(buffer);
    const auto path = catalogPath(request->uri, rest::catalog::uri_content_wildcard);

    // create a temporary location for the file
    auto inwork =
        context->catalog.addFile(path, timestamp, request->content_len);
    if (!inwork.has_value())
        return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);

    // receive the file
    auto fid = inwork.value().open();
    if (rest::receiveOctetStream(request, fid))
    {
        // rename the temporary to the actual file
        ::close(fid);
        inwork.value().done();
    }

    // receiveOctetStream has already set the httpd status
    return httpd_resp_send(response, nullptr, 0);
}

esp_err_t title_handler(httpd_req_t *request)
{
    auto context = reinterpret_cast<Context *>(request->user_ctx);
    auto response = request;

    const auto path = catalogPath(request->uri, rest::catalog::uri_title_wildcard);
    switch (request->method)
    {
    case HTTP_PUT:
    {
        ESP_LOGD(rest::catalog::TAG, "handling PUT TITLE for %s", path.c_str());

        const auto title = rest::getQueryValue(request->uri, "title");
        if (title.has_value())
        {
            if (context->catalog.setTitle(path, title.value()))
                return httpd_resp_send(response, nullptr, 0);
            else
                return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);
        }
        else
            return httpd_resp_send_err(response, HTTPD_400_BAD_REQUEST, nullptr);
    }
    case HTTP_DELETE:
    {
        ESP_LOGD(rest::catalog::TAG, "handling DELETE TITLE for %s", path.c_str());

        if (context->catalog.removeTitle(path))
            return httpd_resp_send(response, nullptr, 0);
        else
            return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);
    }
    default:
        return rest::ILLEGAL_REQUEST(request);
    }
}

static esp_err_t PUT_ICON(httpd_req_t *const request);
esp_err_t icon_handler(httpd_req_t *request)
{
    auto context = reinterpret_cast<Context *>(request->user_ctx);
    auto response = request;

    auto path = catalogPath(request->uri, rest::catalog::uri_icon_wildcard);
    // no icons for the root folder
    if (path.empty() || (path == "/"))
        return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);

    switch (request->method)
    {
    case HTTP_GET:
    {
        ESP_LOGD(rest::catalog::TAG, "handling GET ICON for %s", path.c_str());

        auto data = context->catalog.readIcon(path);
        if (data.has_value())
        {
            rest::sendOctetStream(request, data.value());
            // sendOctetStream has set the response status
            return httpd_resp_send(response, nullptr, 0);
        }
        else
            return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);
    }
    case HTTP_PUT:
    {
        ESP_LOGD(rest::catalog::TAG, "handling PUT ICON for %s", path.c_str());

        return PUT_ICON(request);
    }
    case HTTP_DELETE:
    {
        ESP_LOGD(rest::catalog::TAG, "handling DELETE ICON for %s", path.c_str());

        if (context->catalog.removeIcon(path))
            return httpd_resp_send(response, nullptr, 0);
        else
            return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);
    }
    default:
        return rest::ILLEGAL_REQUEST(request);
    }
}
static esp_err_t PUT_ICON(httpd_req_t *const request)
{
    auto context = reinterpret_cast<Context *>(request->user_ctx);
    auto response = request;

    // validate http header Content-Length
    if (!request->content_len)
    {
        return httpd_resp_send_err(response, HTTPD_411_LENGTH_REQUIRED, nullptr);
    }

    auto path = catalogPath(request->uri, rest::catalog::uri_icon_wildcard);
    // no icons for the root folder
    if (path.empty() || (path == "/"))
        return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);

    auto inwork =
        context->catalog.setIcon(path, request->content_len);
    if (!inwork.has_value())
        return httpd_resp_send_err(response, HTTPD_404_NOT_FOUND, nullptr);

    // receive the file
    auto fid = inwork.value().open();
    if (rest::receiveOctetStream(request, fid))
    {
        // rename the temporary to the actual file
        close(fid);
        inwork.value().done();
    }

    // receiveOctetStream has already set the httpd status
    return httpd_resp_send(response, nullptr, 0);
}
