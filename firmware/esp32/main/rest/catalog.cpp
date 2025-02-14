#include "catalog.hpp"
#include "utils.hpp"

#include <cJSON.h>

#include <esp_log.h>
#include <sdkconfig.h>
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL     CONFIG_FREE_LIBRARY_LOG_LEVEL
using rest::catalog::TAG;

struct Context {
    WebServer& webserver;        
    Catalog& catalog;
};
extern "C" esp_err_t handler(httpd_req_t*);

void rest::catalog::registerHandlers(WebServer& webserver, Catalog& catalog)
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
            .user_ctx = &context
        }
    );
}

enum class UriType {
    ILLEGAL,
    FOLDER,
    FILE,
    ICON,
    TITLE
};
static UriType uriType(const std::string& uri)
{
    if (uri.ends_with("?icon"))
        return UriType::ICON;

    if (uri.contains("?title="))
        return UriType::TITLE;

    // no other queries allowed
    if (uri.contains("?"))
        return UriType::ILLEGAL;

    // fragments not allowed
    if (uri.contains("#"))
        return UriType::ILLEGAL;

    if (uri.back() == '/')
        return UriType::FOLDER;
    else
        return UriType::FILE;
}

// forward declarations
static esp_err_t GET_FOLDER(httpd_req_t* const request);
static esp_err_t DELETE_FOLDER(httpd_req_t* const request);
static esp_err_t GET_FILE(httpd_req_t* const request);
static esp_err_t PUT_FILE(httpd_req_t* const request);
static esp_err_t DELETE_FILE(httpd_req_t* const request);
static esp_err_t GET_ICON(httpd_req_t* const request);
static esp_err_t PUT_ICON(httpd_req_t* const request);
static esp_err_t PUT_TITLE(httpd_req_t* const request);

esp_err_t handler(httpd_req_t* request)
{
    switch(uriType(request->uri))
    {
        case UriType::FOLDER :
        {
            switch(request->method)
            {
                case HTTP_GET : return GET_FOLDER(request);
                case HTTP_DELETE: return DELETE_FOLDER(request);
                default:
                    return rest::ILLEGAL_REQUEST(request);
            }
        }

        case UriType::FILE :
        {
            switch(request->method)
            {
                case HTTP_GET : return GET_FILE(request);
                case HTTP_PUT: return PUT_FILE(request);
                case HTTP_DELETE: return DELETE_FILE(request);

                default:
                    return rest::ILLEGAL_REQUEST(request);
            }
        }

        case UriType::ICON:
        {
            switch(request->method)
            {
                case HTTP_GET : return GET_ICON(request);
                case HTTP_PUT: return PUT_ICON(request);

                default:
                    return rest::ILLEGAL_REQUEST(request);
            }
        }

        case UriType::TITLE:
        {
            switch(request->method)
            {
                case HTTP_PUT: return PUT_TITLE(request);

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

static std::string catalogPath(const char* const requestUri)
{
    // omit the base uri
    auto path =  std::string(requestUri + rest::catalog::uri_wildcard.length() - sizeof('*'));
    // remove query
    auto pos = path.find('?');
    if (pos != std::string::npos) path.erase(pos);
    // remove fragment
    pos = path.find('#');
    if (pos != std::string::npos) path.erase(pos);

    // decode http tokens (in-place)
    rest::httpDecode(path);

    return path;
}

esp_err_t GET_FOLDER(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto folderpath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for FOLDER [/%s]", request->uri, folderpath.c_str());

    if (! context->catalog.hasFolder(folderpath))
        return httpd_resp_send_404(request);

    // send the data
    auto folderInfo = context->catalog.folderInfo(folderpath);
    auto response = cJSON_CreateObject();
    cJSON_AddBoolToObject(response, "locked", folderInfo.isLocked);
    auto subfolders = cJSON_CreateArray();
    for (auto& subfolder : folderInfo.subfolders)
        cJSON_AddItemToArray(subfolders, cJSON_CreateString(subfolder.c_str()));
    cJSON_AddItemToObject(response, "subfolders", subfolders);
    auto files = cJSON_CreateArray();
    for (auto& file : folderInfo.files)
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

esp_err_t DELETE_FOLDER(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto folderpath = catalogPath(request->uri);
    ESP_LOGD(TAG, "handling request[%s] for DELETE FOLDER [/%s]", request->uri, folderpath.c_str());

    if (! context->catalog.hasFolder(folderpath))
        return httpd_resp_send_404(request);

    // requires admin access if this folder or parent folder is locked
    auto path = std::filesystem::path(folderpath);
    if (context->catalog.isLocked(path) || context->catalog.isLocked(path.parent_path()))
    {
        // TODO determine if caller has admin credentials
        if (false /* hasAdminCredentials() */)
            return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    }

    if (context->catalog.removeFolder(folderpath))
        return httpd_resp_sendstr(request, "OK");
    else
        return httpd_resp_send_custom_err(request, "409 - Directory not empty", "failed");
}

esp_err_t GET_FILE(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGD(TAG, "handling request[%s] for GET FILE [/%s]", request->uri, filepath.c_str());

    if (! context->catalog.hasFile(filepath))
        return httpd_resp_send_404(request);

    // set timestamp header
    auto timestamp = rest::timestamp(context->catalog.timestamp(filepath));
    httpd_resp_set_hdr(request, rest::catalog::XFILETIMESTAMP, timestamp.c_str());

    auto fis = context->catalog.readContent(filepath);
    return rest::sendOctetStream(request, fis);
}

esp_err_t PUT_FILE(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGD(TAG, "handling request[%s] for PUT FILE [/%s]", request->uri, filepath.c_str());

    // requires admin access if this folder is locked
    auto path = std::filesystem::path(filepath).parent_path();
    if (context->catalog.isLocked(path))
    {
        // TODO determine if caller has admin credentials
        if (false /* hasAdminCredentials() */)
            return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    }

    // receive the data
    // FIXME should check request->content_len (aka HTTP header CONTENT-LENGTH) to see if it it'll fit

    // check for timestamp in X-FileTimeStamp header
    std::optional<std::filesystem::file_time_type> timestamp = std::nullopt;
    char buffer[21];
    if (ESP_OK == httpd_req_get_hdr_value_str(request, rest::catalog::XFILETIMESTAMP, buffer, sizeof(buffer)))
        timestamp = rest::timestamp(buffer);
    else {
        ESP_LOGW(TAG, "unable to get timestamp header");
        ESP_LOGD(TAG, "X-FileTimeStamp header size was %i", httpd_req_get_hdr_value_len(request, "X-FileTimestamp"));
    }

    auto inwork = context->catalog.addFile(filepath, timestamp);
    if (rest::receiveOctetStream(request, inwork.ofs))
    {
        // complete the transfer by publishing the new file
        inwork.done();
        return httpd_resp_sendstr(request, "OK");
    }
    
    // upon error, rest::receieveOctetStream has already responded to client
    return ESP_OK;
}

esp_err_t DELETE_FILE(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGD(TAG, "handling request[%s] for DELETE FILE [/%s]", request->uri, filepath.c_str());

    if (! context->catalog.hasFile(filepath))
        return httpd_resp_send_404(request);

    if (context->catalog.isLocked(filepath))
    {
        // TODO determine if caller has admin credentials
        if (false /* hasAdminCredentials() */)
            return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    }

    if (context->catalog.removeFile(filepath))
        return httpd_resp_sendstr(request, "OK");
    else
        return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, nullptr);
}

esp_err_t GET_ICON(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for PUT ICON [/%s]", request->uri, filepath.c_str());

    if (! context->catalog.hasFile(filepath))
        return httpd_resp_send_404(request);

    // create a chunk buffer
    std::unique_ptr<char[]> buf(new char[rest::CHUNK_SIZE]);
    if (! buf)
        return httpd_resp_send_custom_err(request, "408 - Too many requests", "try-again");

    auto fis = context->catalog.readIcon(filepath);
    return rest::sendOctetStream(request, fis);
}


esp_err_t PUT_ICON(httpd_req_t* const request)
{
    auto context = reinterpret_cast<Context*>(request->user_ctx);
    const auto filepath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for PUT ICON [/%s]", request->uri, filepath.c_str());

    if (! context->catalog.hasFile(filepath))
        return httpd_resp_send_404(request);

    if (context->catalog.isLocked(filepath))
    {
        // TODO determine if caller has admin credentials
        return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    }

    auto inwork = context->catalog.addIcon(filepath /*, timestamp */);
    if (rest::receiveOctetStream(request, inwork.ofs))
    {
        // complete the transfer by publishing the new file
        inwork.done();
        return httpd_resp_sendstr(request, "OK");
    }
    
    // upon error, rest::receieveOctetStream has already responded to client
    return ESP_OK;
}

esp_err_t PUT_TITLE(httpd_req_t* const request)
{
    return rest::ILLEGAL_REQUEST(request);
    // auto context = reinterpret_cast<Context*>(request->user_ctx);
    // const auto filepath = catalogPath(request->uri);
    // ESP_LOGI(TAG, "handling request[%s] for PUT ICON [/%s]", request->uri, filepath.c_str());

    // if (! context->catalog.hasFile(filepath))
    //     return httpd_resp_send_404(request);

    // if (context->catalog.isLocked(filepath))
    // {
    //     // TODO determine if caller has admin credentials
    //     return httpd_resp_send_err(request, HTTPD_401_UNAUTHORIZED, "folder is locked by admin");
    // }

    // char title[200]; // FIXME get title from query parameter
    // if (context->catalog.setTitle(filepath, title))
    //     return httpd_resp_sendstr(request, "OK");
    // else
    //     return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, nullptr);
}
