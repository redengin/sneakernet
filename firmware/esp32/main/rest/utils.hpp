#pragma once

#include <esp_http_server.h>
#include <esp_err.h>
#include <fstream>

namespace rest {

/// replaces HTTP tokens inplace
void httpDecode(std::string& encoded);


/// HTTP timestamp format
static constexpr char ISO_8601_FORMAT[] = "%FT%T";
void timestamp(const time_t& timestamp, char buffer[20]);


// static constexpr std::size_t CHUNK_SIZE = 1048;
static constexpr std::size_t CHUNK_SIZE = 1450;
esp_err_t sendOctetStream(
    httpd_req_t* const request,
    std::ifstream& fis
);

};
