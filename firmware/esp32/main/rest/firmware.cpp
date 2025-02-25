#include "firmware.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL
using rest::firmware::TAG;

#include <cJSON.h>
#include <esp_ota_ops.h>

#include "utils.hpp"

static esp_err_t GET_FIRMWARE(httpd_req_t *const request);
static esp_err_t PUT_FIRMWARE(httpd_req_t *const request);

void rest::firmware::registerHandlers(WebServer &webserver) {
  // register the handlers with the webserver
  webserver.registerUriHandler(
      httpd_uri_t{.uri = uri.begin(),
                  .method = static_cast<http_method>(HTTP_GET),
                  .handler = GET_FIRMWARE,
                  .user_ctx = nullptr});
  webserver.registerUriHandler(
      httpd_uri_t{.uri = uri.begin(),
                  .method = static_cast<http_method>(HTTP_PUT),
                  .handler = PUT_FIRMWARE,
                  .user_ctx = nullptr});
}

esp_err_t GET_FIRMWARE(httpd_req_t *const request) {
  ESP_LOGD(TAG, "handling request[%s] for GET FIRMWARE", request->uri);

  // turn the firmware info into JSON
  auto data = cJSON_CreateObject();
  cJSON_AddStringToObject(data, "hardware", "ESP32");
  const auto app_description = esp_app_get_description();
  cJSON_AddStringToObject(data, "version", app_description->version);

  // create the response
  char *const response = cJSON_PrintUnformatted(data);
  cJSON_Delete(data);

  if (response == nullptr)
    return httpd_resp_send_custom_err(request, rest::TOO_MANY_REQUESTS,
                                      nullptr);

  // return the data
  httpd_resp_set_type(request, "application/json");
  auto ret = httpd_resp_send(request, response, strlen(response));

  // cleanup json allocations
  cJSON_free(response);
  return ret;
}

// convert semantic string to integer
static uint32_t version(const std::string s) {
  uint32_t ret = 0;
  auto rpos = std::string::npos;  // tracks the position of last '.'
  size_t i = 0;                   // count of how many fields we've read
  do {
    const auto next_rpos = s.rfind('.', rpos);
    int v;
    if (next_rpos == std::string::npos)
      v = std::atoi(s.c_str());
    else
      v = std::atoi(s.substr((next_rpos + 1), (rpos - next_rpos)).c_str());

    ret += v << (i * 4);
    ++i;

    // are we done?
    if (next_rpos == std::string::npos) return ret;
    // find the next dot
    else
      rpos = next_rpos - 1;
  } while (true);
}

static bool isValidUpdate(const esp_partition_t *const partition) {
  // get the udpate info
  esp_app_desc_t update_description;
  if (ESP_OK != esp_ota_get_partition_description(partition, &update_description)) {
    ESP_LOGE(TAG, "unable to get app description for update");
    return false;
  }

  // get the current info
  const auto current_description = esp_app_get_description();

  // update version must be greater than current version
  return version(update_description.version) >
         version(current_description->version);
}

esp_err_t PUT_FIRMWARE(httpd_req_t *const request) {
  ESP_LOGD(TAG, "handling request[%s] for PUT FIRMWARE", request->uri);

  // make sure there is data
  if (request->content_len <= 0)
    return httpd_resp_send_err(request, HTTPD_411_LENGTH_REQUIRED,
                               "Length required");

  // create a chunk buffer
  std::unique_ptr<char[]> chunk(new char[rest::CHUNK_SIZE]);
  if (!chunk)
    return httpd_resp_send_custom_err(request, rest::TOO_MANY_REQUESTS,
                                      nullptr);

  // open an ofs to the next OTA partition
  const esp_partition_t *const partition =
      esp_ota_get_next_update_partition(nullptr);
  esp_ota_handle_t handle;
  if (ESP_OK != esp_ota_begin(partition, request->content_len, &handle)) {
    ESP_LOGW(TAG, "unable to begin firmware update");
    return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR,
                               "unable to start ota update");
  }

  // receive the data
  bool complete = false;
  do {
    const auto received =
        httpd_req_recv(request, chunk.get(), rest::CHUNK_SIZE);
    if (received > 0) {
      if (ESP_OK != esp_ota_write(handle, chunk.get(), received)) {
        ESP_LOGE(TAG, "unable to write ota parition");
        httpd_resp_set_status(request, HTTPD_500);
        break;
      }
    } else if (received == 0) {
      // empty chunk signals completion
      complete = true;
      break;
    } else {
      // socket died while receiving
      ESP_LOGW(TAG, "ota transfer interrupted");
      httpd_resp_set_status(request, HTTPD_408);
      break;
    }
  } while (true);

  // close the ota partition
  if (ESP_OK != esp_ota_end(handle))
    return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR,
                               "unable to close ota partition");

  // abort if transfer failed (above has already set HTTP status)
  if (!complete) return httpd_resp_send(request, nullptr, 0);

  // validate new ota partition
  if (isValidUpdate(partition)) {
    // set us to boot from new partition
    if (ESP_OK != esp_ota_set_boot_partition(partition)) {
      ESP_LOGE(TAG, "unable to set new boot partition");
      return httpd_resp_send_err(request, HTTPD_500_INTERNAL_SERVER_ERROR,
                                 "set boot partition failed.");
    }

    // send the response
    httpd_resp_send(request, nullptr, 0);
    sleep(1); // give it a second to send the response

    // reboot
    ESP_LOGD(TAG, "rebooting to new OTA partition");
    esp_restart();
  } else
    return httpd_resp_send_err(request, HTTPD_403_FORBIDDEN,
                               "firmware version not newer than current");
}
