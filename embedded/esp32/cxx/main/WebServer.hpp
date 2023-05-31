#pragma once
#include "SneakerNet.hpp"
#include <esp_http_server.h>

class WebServer {
public:
    WebServer(SneakerNet&);

    // TODO hide sneakerNet
    SneakerNet sneakerNet;
private:
    httpd_handle_t handle;
    std::string password;
};
