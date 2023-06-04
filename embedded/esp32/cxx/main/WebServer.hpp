#pragma once
#include "SneakerNet.hpp"
#include <esp_http_server.h>

class WebServer {
public:
    const std::string CATALOG_URI = "/catalog";
    const std::string FILES_URI = "/files";
    const std::string FILE_URI = (FILES_URI + "/*");

    WebServer(SneakerNet&);

    // TODO hide sneakerNet
    SneakerNet sneakerNet;
private:
    httpd_handle_t handle;

};
