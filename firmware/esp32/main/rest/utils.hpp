#pragma once

#include <esp_err.h>
#include <esp_http_server.h>

#include <filesystem>
#include <fstream>
#include <optional>

namespace rest {

static constexpr char TAG[] = "rest::utils";  ///< ESP logging tag

/// replaces HTTP tokens inplace
void httpDecode(std::string& encoded);

// returns http decoded query value
std::optional<std::string> getQueryValue(
    const std::string& uri,  ///< raw (not http decoded)
    const std::string& parameter);

/// timestamp format
static constexpr char ISO_8601_Z_FORMAT[] = "%Y-%m-%dT%TZ";
std::string timestamp(const std::filesystem::file_time_type&);
std::filesystem::file_time_type timestamp(const std::string&);

// common HTTP request handlers
esp_err_t ILLEGAL_REQUEST(httpd_req_t* request);
static constexpr char FORBIDDEN[] = "403 Forbidden";
static constexpr char TOO_MANY_REQUESTS[] = "429 Too Many Requests";

/// optimized for sdcard write
static constexpr std::size_t CHUNK_SIZE = CONFIG_FATFS_VFS_FSTAT_BLKSIZE;
/// @brief send chunked HTTP stream
bool sendOctetStream(httpd_req_t* const request, std::ifstream& fis);

/// @brief receive chunked HTTP stream
/// @post set's request's response HTTPD status
bool receiveOctetStream(httpd_req_t* const request, std::ofstream& fos);

};  // namespace rest
