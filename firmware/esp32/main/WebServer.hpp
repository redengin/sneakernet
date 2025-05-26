#pragma once

#include <esp_http_server.h>
#include <esp_https_server.h>


class WebServer
{
public:
    static constexpr char TAG[] = "WebServer";    ///< ESP logging tag

    static constexpr char CAPTIVE_PORTAL_URI[] = "/captive-portal-api";

    /// Creates both an HTTP and HTTPS server
    WebServer(const size_t max_sockets);

    /// registers handler for both HTTP and HTTPS servers
    /// @detail see isHttpsRequest for handler to determine which
    void registerUriHandler(const httpd_uri_t& uri);

    bool isHttpsRequest(const httpd_req_t* request)
    { return request->handle == httpsHandle; }

private:
    httpd_handle_t httpsHandle;
    static constexpr size_t MAX_HTTPS_URI_HANDLERS = 9;

    httpd_handle_t httpHandle;
    static constexpr size_t MAX_HTTP_URI_HANDLERS = 11;
};
