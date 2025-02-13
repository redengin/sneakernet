#include "utils.hpp"

#include <esp_log.h>
#include <memory>
#include <algorithm>
#include <ctime>


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

void rest::timestamp(const time_t& timestamp, char buffer[20])
{
    std::tm tm;
    gmtime_r(&timestamp, &tm);
    strftime(buffer, sizeof(buffer), rest::ISO_8601_FORMAT, &tm);
}





esp_err_t rest::sendOctetStream(httpd_req_t* const request, std::ifstream& fis)
{
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(rest::CHUNK_SIZE);
    if (! buffer)
        return httpd_resp_send_err(request, HTTPD_408_REQ_TIMEOUT, "Too many requests (try again)");

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
