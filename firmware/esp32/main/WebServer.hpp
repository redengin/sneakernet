#pragma once

#include <esp_http_server.h>
#include <esp_https_server.h>


class WebServer
{
public:
    static constexpr char TAG[] = "WebServer";    ///< ESP logging tag

    static constexpr char INDEX_URI[] = "/app/";

    /// Creates both an HTTP and HTTPS server
    WebServer(const size_t max_sockets);

    /// registers handler for both HTTP and HTTPS servers
    /// @detail see isHttpsRequest for hanlder to determine which
    void registerUriHandler(const httpd_uri_t& uri);

    bool isHttpsRequest(const httpd_req_t* request)
    { return request->handle == httpsHandle; }

private:
    httpd_handle_t httpHandle;
    httpd_handle_t httpsHandle;
};
