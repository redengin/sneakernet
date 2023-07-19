#pragma once
#include "SneakerNet.hpp"
#include <esp_http_server.h>

class WebServer {
public:
    const std::string CATALOG_URI = SneakerNet::CATALOG_DIR;
    const std::string CATALOG_FILE_URI = CATALOG_URI + "/*";

    WebServer(SneakerNet&);

    // TODO hide sneakerNet
    SneakerNet sneakerNet;
private:
    httpd_handle_t handle;
};
