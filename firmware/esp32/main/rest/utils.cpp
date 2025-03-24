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
  for (auto pos = encoded.find('%'); pos != std::string::npos;
       pos = encoded.find(pos + 1, '%')) {
    if ((pos + 2) <= encoded.length()) {
      const char utf8[] = {encoded[pos + 1], encoded[pos + 2]};
      const char actualChar = static_cast<char>(std::stoi(utf8, 0, 16));
      if (actualChar)
        encoded.replace(pos, 3, 1, actualChar);
      else
        // invalid char, erase it instead
        encoded.erase(pos, 3);
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

  // send the data
  do {
    const auto sz = fis.readsome(buffer.get(), rest::CHUNK_SIZE);
    if (ESP_OK != httpd_resp_send_chunk(request, buffer.get(), sz)) {
      // file transfer interrupted
      httpd_resp_set_status(request, HTTPD_408);
      return false;
    }

    // send is complete once we've sent a 0 length chunk
    if (sz == 0) return true;

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

  // receive the data
  while (fos.good()) {
    const auto received =
        httpd_req_recv(request, buffer.get(), rest::CHUNK_SIZE);

    if (received < 0) {
      ESP_LOGD(TAG, "socket closed during upload");
      httpd_resp_set_status(request, HTTPD_408);
      return false;
    }

    // complete upon empty chunk
    if (received == 0) {
      ESP_LOGD(TAG, "received %d bytes", total_sz);
      return total_sz >= request->content_len;
    }

    fos.write(buffer.get(), received);
    total_sz += received;
  };

  // fos went bad
  ESP_LOGE(TAG, "output stream failed [%s]", strerror(errno));
  httpd_resp_set_status(request, HTTPD_500);
  return false;
}
