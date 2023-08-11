#pragma once
#include "SneakerNet.hpp"
#include <esp_http_server.h>

class WebServer {
public:
    WebServer(SneakerNet&);
    SneakerNet sneakernet;

private:
    httpd_handle_t handle;
};
