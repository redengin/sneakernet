#include "utils.hpp"

#include <memory>
#include <algorithm>
#include <time.h>

#include <esp_log.h>
#include <sdkconfig.h>
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL     CONFIG_FREE_LIBRARY_LOG_LEVEL
using rest::TAG;


void rest::httpDecode(std::string& encoded)
{
    // replace '+' with space
    std::replace(encoded.begin(), encoded.end(), '+', ' ');

    // translate UTF-8
    for(auto pos = encoded.find('%'); pos != std::string::npos; pos = encoded.find(pos+1, '%'))
    {
        if ((pos + 2) <= encoded.length())
        {
            const char utf8[] = {encoded[pos+1], encoded[pos+2]};
            const char actualChar = static_cast<char>(std::stoi(utf8, 0, 16));
            if (actualChar)
                encoded.replace(pos, 3, 1, actualChar);
            else
                // invalid char, erase it instead
                encoded.erase(pos, 3);
        }
        else {
            // illegal uri, dropping the remaining characters
            encoded.erase(pos);
            break;
        }
    }
}

std::string rest::timestamp(const std::filesystem::file_time_type& timestamp)
{
    const auto timestamp_s = std::chrono::system_clock::to_time_t(
        std::chrono::file_clock::to_sys(timestamp)
    );
    ESP_LOGD(TAG, "timestamp from file_time_type is %lli", timestamp_s);

    std::tm tm;
    gmtime_r(&timestamp_s, &tm);

    std::stringstream ss;
    ss << std::put_time(&tm, rest::ISO_8601_Z_FORMAT);
    if (ss.fail())
    {
        ESP_LOGW(TAG, "illegal rest::ISO_8601_Z_FORMAT");
        return "";
    }

    return ss.str();
}

std::optional<std::filesystem::file_time_type> rest::timestamp(const std::string& timestamp)
{
    std::istringstream ss(timestamp);
    std::tm tm{};
    ss >> std::get_time(&tm, ISO_8601_Z_FORMAT);
    if (ss.fail())
    {
        ESP_LOGW(TAG, "failed to parse timestamp [%s]", timestamp.c_str());
        return std::nullopt;
    }

    std::time_t timestamp_s = std::mktime(&tm);
    ESP_LOGD(TAG, "timestamp from string is %lli", timestamp_s);

    return std::filesystem::file_time_type::clock::from_sys(
        std::chrono::system_clock::from_time_t(timestamp_s)
    );
}

esp_err_t rest::ILLEGAL_REQUEST(httpd_req_t* request)
{
    return httpd_resp_send_err(request, HTTPD_400_BAD_REQUEST, nullptr);
}
esp_err_t rest::TOO_MANY_REQUESTS(httpd_req_t* request)
{
    return httpd_resp_send_custom_err(request, "429 Too many requests", "try-again");
}

esp_err_t rest::sendOctetStream(httpd_req_t* const request, std::ifstream& fis)
{
    // create a chunk buffer
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(rest::CHUNK_SIZE);
    if (! buffer)
        return TOO_MANY_REQUESTS(request);

    // set the response mime type
    httpd_resp_set_type(request, "application/octet-stream");

    // send the data
    do {
        const size_t sz = fis.readsome(buffer.get(), rest::CHUNK_SIZE);
        if (ESP_OK != httpd_resp_send_chunk(request, buffer.get(), sz))
            // file transfer interrupted
            return ESP_FAIL;

        // send is complete once we've sent a 0 length chunk
        if (sz == 0)
            return ESP_OK;

    } while(true);
}

bool rest::receiveOctetStream(httpd_req_t* const request, std::ofstream& fos)
{
    // create a chunk buffer
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(rest::CHUNK_SIZE);
    if (! buffer)
    {
        ESP_LOGD(TAG, "unable to create recieve buffer");
        TOO_MANY_REQUESTS(request);
        return false;
    }

    // receive the data
    while (fos.good())
    {
        const int received = httpd_req_recv(request, buffer.get(), rest::CHUNK_SIZE);
        if (received < 0)
        {
            ESP_LOGD(TAG, "socket closed during upload");
            return false;
        }

        if (received == 0)
        {
            ESP_LOGD(TAG, "upload completed");
            return true;
        }

        fos.write(buffer.get(), received);
    };

    ESP_LOGD(TAG, "invalid output stream [%s]", strerror(errno));
    httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR, nullptr);
    return false;
}
