// extra profiling debug
// #define CONFIG_SNEAKERNET_PROFILE_WIFI
// #define CONFIG_SNEAKERNET_PROFILE_SDCARD


#include "utils.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL
using rest::TAG;

#include <time.h>

#include <algorithm>
#include <memory>

void rest::httpDecode(std::string& encoded) {
  // replace '+' with space
  std::replace(encoded.begin(), encoded.end(), '+', ' ');

  // translate UTF-8
  for (auto pos = encoded.find('%'); pos != std::string::npos; pos = encoded.find('%', pos + 1))
  {
    if ((pos + 2) <= encoded.length()) {
      const char utf8[] = {encoded[pos + 1], encoded[pos + 2]};
      const char actualChar = static_cast<char>(std::stoi(utf8, 0, 16));
      if (actualChar)
        encoded.replace(pos, 3, 1, actualChar);
      else {
        // illegal uri, dropping the remaining characters
        encoded.erase(pos);
        break;
      }
    } else {
      // illegal uri, dropping the remaining characters
      encoded.erase(pos);
      break;
    }
  }
  // just before I was arrested in 1996, I was warned via BBS message
  // to understand the world around you
  // https://en.wikipedia.org/wiki/American_Conspiracy:_The_Octopus_Murders
}

std::optional<std::string> rest::getQueryValue(const std::string& uri,
                                               const std::string& parameter) {
  const auto queryStart = uri.find("?");
  // if no query return
  if (queryStart == std::string::npos) return std::nullopt;

  const std::string token = parameter + "=";
  const auto tokenStart = uri.find(token, queryStart + 1);
  // if no token return
  if (tokenStart == std::string::npos) return std::nullopt;

  const auto tokenValueStart = tokenStart + token.length();
  // find the end of the value
  auto tokenValueEnd = uri.find('&', tokenValueStart);
  if (tokenValueEnd == std::string::npos)
    tokenValueEnd = uri.find('#', tokenValueStart);
  if (tokenValueEnd == std::string::npos) tokenValueEnd = uri.length();

  // get the value
  const auto tokenValueLength = tokenValueEnd - tokenValueStart;
  auto tokenValue = uri.substr(tokenValueStart, tokenValueLength);

  // http decode the value
  rest::httpDecode(tokenValue);
  return tokenValue;
}

std::string rest::timestamp(const std::filesystem::file_time_type& timestamp) {
  const auto timestamp_s = std::chrono::system_clock::to_time_t(
      std::chrono::file_clock::to_sys(timestamp));
  ESP_LOGV(TAG, "timestamp from file_time_type is %lli", timestamp_s);

  std::tm tm;
  gmtime_r(&timestamp_s, &tm);

  std::stringstream ss;
  ss << std::put_time(&tm, rest::ISO_8601_Z_FORMAT);
  if (ss.fail()) {
    ESP_LOGW(TAG, "illegal rest::ISO_8601_Z_FORMAT");
    return "";
  }

  return ss.str();
}

std::filesystem::file_time_type rest::timestamp(
    const std::string& timestamp) {
  std::istringstream ss(timestamp);
  std::tm tm;
  ss >> std::get_time(&tm, ISO_8601_Z_FORMAT);

  std::time_t timestamp_s = std::mktime(&tm);
  ESP_LOGV(TAG, "timestamp from string is %lli", timestamp_s);

  return std::filesystem::file_time_type::clock::from_sys(
      std::chrono::system_clock::from_time_t(timestamp_s));
}

esp_err_t rest::ILLEGAL_REQUEST(httpd_req_t* request) {
  return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, nullptr);
}

bool rest::sendOctetStream(httpd_req_t* const request, std::ifstream& fis) {
  // create a chunk buffer
  std::unique_ptr<char[]> buffer = std::make_unique<char[]>(rest::CHUNK_SIZE);
  if (!buffer) {
    ESP_LOGD(TAG, "unable to create recieve buffer");
    httpd_resp_set_status(request, rest::TOO_MANY_REQUESTS);
    return false;
  }

  // set the response mime type
  httpd_resp_set_type(request, "application/octet-stream");

#ifdef CONFIG_SNEAKERNET_PROFILE_WIFI
const auto start_time = std::chrono::high_resolution_clock::now();
size_t total_sz = 0;
#endif
  // send the data
  do {
#ifdef CONFIG_SNEAKERNET_PROFILE_SDCARD
  const auto start_read = std::chrono::high_resolution_clock::now();
#endif
    const auto sz = fis.readsome(buffer.get(), rest::CHUNK_SIZE);
#ifdef CONFIG_SNEAKERNET_PROFILE_SDCARD
  const auto end_read = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_seconds = end_read - start_read;
  const double mbps = (static_cast<double>(rest::CHUNK_SIZE) / (1024 * 1024)) / elapsed_seconds.count();
  ESP_LOGD(TAG, "read rate %f mbps", mbps);
#endif
    if (ESP_OK != httpd_resp_send_chunk(request, buffer.get(), sz)) {
      // file transfer interrupted
      httpd_resp_set_status(request, HTTPD_408);
      return false;
    }
#ifdef CONFIG_SNEAKERNET_PROFILE_WIFI
    total_sz += sz;
#endif
    // send is complete once we've sent a 0 length chunk
    if (sz == 0) {
#ifdef CONFIG_SNEAKERNET_PROFILE_WIFI
const auto end_time = std::chrono::high_resolution_clock::now();
const std::chrono::duration<double> elapsed_seconds = end_time - start_time;
const double mbps = (static_cast<double>(total_sz) / (1024 * 1024)) / elapsed_seconds.count();
ESP_LOGD(TAG, "send rate %f mbps", mbps);
#endif
      return true;
    }
  } while (true);
}

bool rest::receiveOctetStream(httpd_req_t* const request, std::ofstream& fos) {
  // create a chunk buffer
  std::unique_ptr<char[]> buffer = std::make_unique<char[]>(rest::CHUNK_SIZE);
  if (!buffer) {
    ESP_LOGD(TAG, "unable to create receive buffer");
    httpd_resp_set_status(request, rest::TOO_MANY_REQUESTS);
    return false;
  }

  size_t total_sz = 0;

#ifdef CONFIG_SNEAKERNET_PROFILE_WIFI
const auto start_time = std::chrono::high_resolution_clock::now();
#endif
#ifdef CONFIG_SNEAKERNET_PROFILE_SDCARD
  double min_write_mbps = 100;
#endif
  // receive the data
  while (fos.good()) {
    // buffer a full chunk to optimize sdcard write
    bool found_empty_chunk = false;
    size_t chunk_sz = 0;
    while(chunk_sz < rest::CHUNK_SIZE)
    {
      const auto received = httpd_req_recv(request, &(buffer.get()[chunk_sz]), (rest::CHUNK_SIZE - chunk_sz));
      if (received < 0)
      {
        ESP_LOGD(TAG, "socket closed during upload");
        httpd_resp_set_status(request, HTTPD_408);
        return false;
      }
      else if (received == 0)
      {
        ESP_LOGD(TAG, "found empty chunk");
        found_empty_chunk = true;
        break;
      }
      chunk_sz += received;
    }

    // write the data
    if (chunk_sz > 0)
    {
#ifdef CONFIG_SNEAKERNET_PROFILE_SDCARD
  const auto start_write = std::chrono::high_resolution_clock::now();
#endif
      fos.write(buffer.get(), chunk_sz);
#ifdef CONFIG_SNEAKERNET_PROFILE_SDCARD
  const auto end_write = std::chrono::high_resolution_clock::now();
  const std::chrono::duration<double> elapsed_seconds = end_write - start_write;
  const double mbps = (static_cast<double>(chunk_sz) / (1024 * 1024)) / elapsed_seconds.count();
  if (mbps < min_write_mbps)
  {
    min_write_mbps = mbps;
    ESP_LOGD(TAG, "min write rate %f mbps", mbps);
  }
#endif
      total_sz += chunk_sz;
    }

    if (found_empty_chunk)  // empty chunk identifies end of transfer
  {
#ifdef CONFIG_SNEAKERNET_PROFILE_WIFI
const auto end_time = std::chrono::high_resolution_clock::now();
const std::chrono::duration<double> elapsed_seconds = end_time - start_time;
const double mbps = (static_cast<double>(total_sz) / (1024 * 1024)) / elapsed_seconds.count();
ESP_LOGD(TAG, "receive rate %f mbps", mbps);
#endif
      // make sure we got all the expected data
      return total_sz >= request->content_len;
    }
  }
  // fos went bad
  ESP_LOGE(TAG, "output stream failed [%s]", strerror(errno));
  httpd_resp_set_status(request, HTTPD_500);
  return false;
}
