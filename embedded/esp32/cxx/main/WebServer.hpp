#include <esp_http_server.h>

class WebServer {
public:
    WebServer();

private:
    httpd_handle_t handle;
};