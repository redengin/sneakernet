#pragma once
#include "SneakerNet.hpp"
#include <esp_https_server.h>

class AdminWebServer {
public:
    AdminWebServer(SneakerNet&);
    SneakerNet sneakernet;

private:
    httpd_handle_t handle;
};
