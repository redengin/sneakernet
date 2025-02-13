#include "catalog.hpp"
#include "utils.hpp"

#include <esp_log.h>
using rest::catalog::TAG;
#include <cJSON.h>


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
static esp_err_t ILLEGAL_REQUEST(httpd_req_t* const request);
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
                    return ILLEGAL_REQUEST(request);
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
                    return ILLEGAL_REQUEST(request);
            }
        }

        case UriType::ICON:
        {
            switch(request->method)
            {
                case HTTP_GET : return GET_ICON(request);
                case HTTP_PUT: return PUT_ICON(request);

                default:
                    return ILLEGAL_REQUEST(request);
            }
        }

        case UriType::TITLE:
        {
            switch(request->method)
            {
                case HTTP_PUT: return PUT_TITLE(request);

                default:
                    return ILLEGAL_REQUEST(request);
            }
        }

        default:
        {
            ESP_LOGI(TAG, "unhandled request %s", request->uri);
            return ILLEGAL_REQUEST(request);
        }
    }
}

static esp_err_t ILLEGAL_REQUEST(httpd_req_t* request)
{
    return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, nullptr);
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
    const Context& context = reinterpret_cast<const Context&>(request->user_ctx);
    const auto folderpath = catalogPath(request->uri);
    ESP_LOGI(TAG, "handling request[%s] for FOLDER [/%s]", request->uri, folderpath.c_str());

    if (! context.catalog.hasFolder(folderpath))
        return httpd_resp_send_404(request);

    // // send the data
    // auto subfolders = cJSON_CreateArray();
    // auto files = cJSON_CreateArray();
    // for (auto& entry : context.catalog.folderIterator(folderpath))
    // {
    //     if (Catalog::isValid(entry.path()))
    //     {
    //         if (entry.is_directory())
    //             cJSON_AddItemToArray(subfolders, cJSON_CreateString(entry.path().filename().c_str()));

    //         else if (entry.is_regular_file())
    //         {
    //             auto fileInfo = cJSON_CreateObject();
    //             cJSON_AddItemToObject(fileInfo, "name", cJSON_CreateString(entry.path().filename().c_str()));
    //             cJSON_AddNumberToObject(fileInfo, "size", entry.file_size());

    //             auto fileTime = entry.last_write_time().time_since_epoch().count();
    //             char buffer[20];
    //             rest::timestamp(fileTime, buffer);
    //             cJSON_AddItemToObject(fileInfo, "timestamp", cJSON_CreateString(buffer));

    //             auto filepath = (std::filesystem::path(folderpath) / entry.path().filename()).string();
    //             auto title = context.catalog.getTitle(filepath);
    //             if (title)
    //                 cJSON_AddItemToObject(fileInfo, "timestamp", cJSON_CreateString(title.value().c_str()));

    //             cJSON_AddBoolToObject(fileInfo, "hasIcon", context.catalog.hasIcon(filepath));

    //             cJSON_AddItemToArray(files, fileInfo);
    //         }
    //     }
    // }
    // auto response = cJSON_CreateObject();
    // cJSON_AddBoolToObject(response, "locked", context.catalog.isLocked(folderpath));
    // cJSON_AddItemToObject(response, "subfolder", subfolders);
    // cJSON_AddItemToObject(response, "files", files);

    // char *const data = cJSON_PrintUnformatted(response);
    // httpd_resp_set_type(request, "application/json");
    // httpd_resp_send(request, data, strlen(data));

    // cJSON_Delete(response);
    // cJSON_free(data);

    return ESP_OK;
}

esp_err_t DELETE_FOLDER(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}

esp_err_t GET_FILE(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}

esp_err_t PUT_FILE(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}

esp_err_t DELETE_FILE(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}

esp_err_t GET_ICON(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}


esp_err_t PUT_ICON(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}

esp_err_t PUT_TITLE(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}































esp_err_t GET(httpd_req_t* const request)
{
    switch(uriType(request->uri))
    {
        case UriType::FOLDER: return GET_FOLDER(request);

        // case UriType::FILE: {
        //     const auto filepath = catalogPath(request->uri);
        //     ESP_LOGD(TAG, "handling request[%s] for FILE [%s]", request->uri, filepath.c_str());

        //     if (! context.catalog.hasFile(filepath))
        //         return httpd_resp_send_404(request);

        //     // set the headers
        //     char buffer[20];
        //     rest::timestamp(context.catalog.timestamp(filepath), buffer);
        //     httpd_resp_set_hdr(request, "X-FileTimestamp", buffer);

        //     // send the data
        //     auto fis = context.catalog.readContent(filepath);
        //     if (! fis.is_open())
        //         return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try again)");
        //     return rest::sendOctetStream(request, fis);

        //     // set headers
        //     auto timestamp = context.catalog.timestamp(uri);
        //     std::tm tm;
        //     gmtime_r(&timestamp, &tm);
        //     char headerField[30];
        //     strftime(headerField, sizeof(headerField), rest::ISO_8601_FORMAT, &tm);
        //     httpd_resp_set_hdr(request, "X-FileTimestamp", headerField);

        default: return ESP_FAIL;
    }
}

esp_err_t PUT(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}

esp_err_t DELETE(httpd_req_t* const request)
{
    // TODO
    return ESP_FAIL;
}

