#include "catalog.hpp"
#include "utils.hpp"

#include <cJSON.h>

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL
using rest::catalog::TAG;

struct Context
{
    WebServer &webserver;
    Catalog &catalog;
};
extern "C" esp_err_t handler(httpd_req_t *);

void rest::catalog::registerHandlers(WebServer &webserver, Catalog &catalog)
{
    static Context context{
        .webserver = webserver,
        .catalog = catalog,
    };

    webserver.registerUriHandler(
        httpd_uri_t{
            .uri = uri_wildcard.begin(),
            .method = static_cast<http_method>(HTTP_ANY),
            .handler = handler,
            .user_ctx = &context});
}

enum class UriType
{
    ILLEGAL,
    FOLDER,
    FILE,
    ICON,
    TITLE
};
static UriType uriType(const std::string &uri)
{
    // fragments not allowed
    if (uri.contains("#"))
        return UriType::ILLEGAL;

    if (uri.ends_with("?icon"))
        return UriType::ICON;

    if (uri.contains("?title="))
        return UriType::TITLE;

    // no other queries allowed
    if (uri.contains("?"))
        return UriType::ILLEGAL;

    // use final character to determine if folder or file
    if (uri.back() == '/')
        return UriType::FOLDER;
    else
        return UriType::FILE;
}

// forward declarations
static esp_err_t PUT_FOLDER(httpd_req_t *const request);
static esp_err_t GET_FOLDER(httpd_req_t *const request);
static esp_err_t DELETE_FOLDER(httpd_req_t *const request);
static esp_err_t PUT_FILE(httpd_req_t *const request);
static esp_err_t GET_FILE(httpd_req_t *const request);
static esp_err_t DELETE_FILE(httpd_req_t *const request);
static esp_err_t PUT_ICON(httpd_req_t *const request);
static esp_err_t GET_ICON(httpd_req_t *const request);
static esp_err_t PUT_TITLE(httpd_req_t *const request);

esp_err_t handler(httpd_req_t *request)
{
    switch (uriType(request->uri))
    {
    case UriType::FOLDER:
    {
        switch (request->method)
        {
        case HTTP_PUT:
            return PUT_FOLDER(request);
        case HTTP_GET:
            return GET_FOLDER(request);
        case HTTP_DELETE:
            return DELETE_FOLDER(request);
        default:
            return rest::ILLEGAL_REQUEST(request);
        }
    }

    case UriType::FILE:
    {
        switch (request->method)
        {
        case HTTP_PUT:
            return PUT_FILE(request);
        case HTTP_GET:
            return GET_FILE(request);
        case HTTP_DELETE:
            return DELETE_FILE(request);

        default:
            return rest::ILLEGAL_REQUEST(request);
        }
    }

    case UriType::ICON:
    {
        switch (request->method)
        {
        case HTTP_PUT:
            return PUT_ICON(request);
        case HTTP_GET:
            return GET_ICON(request);

        default:
            return rest::ILLEGAL_REQUEST(request);
        }
    }

    case UriType::TITLE:
    {
        switch (request->method)
        {
        case HTTP_PUT:
            return PUT_TITLE(request);

        default:
            return rest::ILLEGAL_REQUEST(request);
        }
    }

    default:
    {
        ESP_LOGI(TAG, "unhandled request %s", request->uri);
        return rest::ILLEGAL_REQUEST(request);
    }
    }
}

static std::string catalogPath(const char *const requestUri)
{
    // omit the base uri
    auto path = std::string(requestUri + rest::catalog::uri_wildcard.length() - sizeof('*'));
    // remove query
    auto pos = path.find('?');
    if (pos != std::string::npos)
        path.erase(pos);
    // remove fragment
    pos = path.find('#');
    if (pos != std::string::npos)
        path.erase(pos);

    // decode http tokens (in-place)
    rest::httpDecode(path);

    return path;
}

esp_err_t PUT_FOLDER(httpd_req_t *const request)
{
    // get the path portion of the URI
    const auto folderpath = catalogPath(request->uri);
    ESP_LOGD(TAG, "handling request[%s] for PUT FOLDER[%s]", request->uri, folderpath.c_str());

    auto context = reinterpret_cast<Context *>(request->user_ctx);

    if (context->catalog.addFolder(folderpath))
        return ESP_OK;
    else
        return httpd_resp_send_err(request, HTTPD_403_FORBIDDEN, "unable to create folder");
}

esp_err_t GET_FOLDER(httpd_req_t *const request)
{
    auto context = reinterpret_cast<Context *>(request->user_ctx);
    const auto folderpath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for FOLDER [/%s]", request->uri, folderpath.c_str());

    if (!context->catalog.hasFolder(folderpath))
        return httpd_resp_send_404(request);

    // send the data
    auto folderInfo = context->catalog.folderInfo(folderpath);
    auto response = cJSON_CreateObject();
    cJSON_AddBoolToObject(response, "locked", folderInfo.isLocked);
    auto subfolders = cJSON_CreateArray();
    for (auto &subfolder : folderInfo.subfolders)
        cJSON_AddItemToArray(subfolders, cJSON_CreateString(subfolder.c_str()));
    cJSON_AddItemToObject(response, "subfolders", subfolders);
    auto files = cJSON_CreateArray();
    for (auto &file : folderInfo.files)
    {
        auto fileInfo = cJSON_CreateObject();
        cJSON_AddItemToObject(fileInfo, "name", cJSON_CreateString(file.name.c_str()));
        cJSON_AddNumberToObject(fileInfo, "size", file.size);
        cJSON_AddItemToObject(fileInfo, "timestamp", cJSON_CreateString(rest::timestamp(file.timestamp).c_str()));
        if (file.title)
            cJSON_AddItemToObject(fileInfo, "timestamp", cJSON_CreateString(file.title.value().c_str()));
        cJSON_AddBoolToObject(fileInfo, "hasIcon", file.hasIcon);

        cJSON_AddItemToArray(files, fileInfo);
    }
    cJSON_AddItemToObject(response, "files", files);

    char *const data = cJSON_PrintUnformatted(response);
    cJSON_Delete(response);

    if (data == nullptr)
        return rest::TOO_MANY_REQUESTS(request);

    httpd_resp_set_type(request, "application/json");
    auto ret = httpd_resp_send(request, data, strlen(data));
    cJSON_free(data);
    return ret;
}

esp_err_t DELETE_FOLDER(httpd_req_t *const request)
{
    // get the path portion of the URI
    const auto folderpath = catalogPath(request->uri);
    ESP_LOGD(TAG, "handling request[%s] for DELETE FOLDER[%s]", request->uri, folderpath.c_str());

    auto context = reinterpret_cast<Context *>(request->user_ctx);

    if (context->catalog.removeFolder(folderpath))
        return ESP_OK;
    else
        return httpd_resp_send_err(request, HTTPD_403_FORBIDDEN, "unable to remove folder");
}

esp_err_t PUT_FILE(httpd_req_t *const request)
{
    const auto filepath = catalogPath(request->uri);
    ESP_LOGD(TAG, "handling request[%s] for PUT FILE[%s]", request->uri, filepath.c_str());

    // check for content size in header
    if (!request->content_len)
        return httpd_resp_send_err(request, HTTPD_403_FORBIDDEN, "Content-Length header required");

    // check for timestamp in header
    char buffer[rest::catalog::ISO8601_MAX_LENGTH]; // Content-Length string must be shorter
    if (ESP_OK != httpd_req_get_hdr_value_str(request, rest::catalog::XFILETIMESTAMP, buffer, sizeof(buffer)))
        return httpd_resp_send_err(request, HTTPD_403_FORBIDDEN, "X-timestamp header required");
    // convert timestamp header to internal representation
    const auto timestamp = rest::timestamp(buffer);

    auto context = reinterpret_cast<Context *>(request->user_ctx);

    // receive the data into a temporary location
    auto inwork = context->catalog.addFile(filepath, timestamp, request->content_len);
    if (rest::receiveOctetStream(request, inwork.ofs))
        inwork.done();

    // upon error, rest::receieveOctetStream has already responded to client
    return ESP_OK;
}

esp_err_t GET_FILE(httpd_req_t *const request)
{
    const auto filepath = catalogPath(request->uri);
    ESP_LOGD(TAG, "handling request[%s] for GET FILE[%s]", request->uri, filepath.c_str());

    auto context = reinterpret_cast<Context *>(request->user_ctx);

    // send the file content
    auto fis = context->catalog.readContent(filepath);
    return rest::sendOctetStream(request, fis);
}

esp_err_t DELETE_FILE(httpd_req_t *const request)
{
    const auto filepath = catalogPath(request->uri);
    ESP_LOGD(TAG, "handling request[%s] for DELETE FILE[%s]", request->uri, filepath.c_str());

    auto context = reinterpret_cast<Context *>(request->user_ctx);

    if (context->catalog.removeFolder(filepath))
        return ESP_OK;
    else
        return httpd_resp_send_err(request, HTTPD_403_FORBIDDEN, "unable to remove file");
}

esp_err_t GET_ICON(httpd_req_t *const request)
{
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for PUT ICON [%s]", request->uri, filepath.c_str());

    auto context = reinterpret_cast<Context *>(request->user_ctx);

    auto fis = context->catalog.readIcon(filepath);
    return rest::sendOctetStream(request, fis);
}

esp_err_t PUT_ICON(httpd_req_t *const request)
{
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for PUT ICON [%s]", request->uri, filepath.c_str());

    auto context = reinterpret_cast<Context *>(request->user_ctx);

    auto inwork = context->catalog.addIcon(filepath);
    if (rest::receiveOctetStream(request, inwork.ofs))
        inwork.done();

    // upon error, rest::receieveOctetStream has already responded to client
    return ESP_OK;
}

esp_err_t PUT_TITLE(httpd_req_t *const request)
{
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for PUT TITLE [%s]", request->uri, filepath.c_str());

    // get the title value from request
    auto title = rest::getQueryValue(request->uri, "title");
    if (! title.has_value())
        return httpd_resp_send_err(request, HTTPD_403_FORBIDDEN, "unable to set title");

    auto context = reinterpret_cast<Context *>(request->user_ctx);

    if (context->catalog.setTitle(filepath, title.value()))
        return ESP_OK;
    else
        return httpd_resp_send_err(request, HTTPD_403_FORBIDDEN, "unable to set title");
}
