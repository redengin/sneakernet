#include "WifiAccessPoint.hpp"

#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

#include <esp_wifi.h>
#include <freertos/task.h>
#include <hal/efuse_hal.h>
#include <nvs_flash.h>
static void dns_service_task(void *pvParameters);
#include <lwip/sockets.h>

// #include <cstring>

WifiAccessPoint::WifiAccessPoint() {
  // tune down log chatter
  esp_log_level_set("wifi_init", ESP_LOG_WARN);

  // start the network stack
  ESP_ERROR_CHECK(esp_netif_init());

  // configure netif for wifi access point
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  // initialize wifi
  {
    const wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  }

  // configure wifi
  {
    wifi_config_t wifi_config;
    ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_AP, &wifi_config));
    // set SSID
    uint8_t mac[6];
    efuse_hal_get_mac(mac);
    wifi_config.ap.ssid_len =
        snprintf(reinterpret_cast<char *>(&wifi_config.ap.ssid),
                 sizeof(wifi_config.ap.ssid), "SneakerNet %X%X%X%X%X%X", mac[5],
                 mac[4], mac[3], mac[2], mac[1], mac[0]);
    // randomize channel by mac
    srand(*reinterpret_cast<uint32_t*>(mac + 2));
    wifi_config.ap.channel = (rand() % (CONFIG_SNEAKERNET_WIFI_CHANNEL_MAX + 1));

    ESP_ERROR_CHECK(esp_wifi_set_config(static_cast<wifi_interface_t>(ESP_IF_WIFI_AP), &wifi_config));
  }

  // handle DNS requests, redirecting to captive portal
  assert(pdPASS == xTaskCreate(
                       dns_service_task, "dns_service",
                       4096 /* stack depth */,  // TODO reduce to minimum
                       nullptr /* parameters (none) */,
                       tskIDLE_PRIORITY /* priority */,
                       nullptr /* handle (not used) */
                       ));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_LOGI(TAG, "started");
}

namespace dns {
#ifndef __GNUC__
#error requires GCC bit field packing.
#endif
#ifdef BITS_BIG_ENDIAN
#error requires bit fields to be packed little-endian (LSB first)
#endif

enum type {
  request = 0,
  response = 1,
};

enum opcode {
  query = 0,
  iquery = 1,  ///< inverse query
  status = 2,  ///< server status
};

enum rcode {
  ok = 0,
  query_err = 1,     ///< query format error
  server_err = 2,    ///< internal issue
  name_err = 3,      ///< name not recognized
  request_fail = 4,  ///< request not supported
  ignored = 5,       ///< service ignored the request
};

typedef struct __attribute__((packed)) {
  uint16_t id;  ///< unique id
  type qr : 1;
  opcode op : 4;
  bool aa : 1;         ///< authoritative result (response only)
  bool tc : 1;         ///< truncated message
  bool rd : 1;         ///< recursion desired
  bool ra : 1;         ///< recursion available (response only)
  uint8_t UNUSED : 3;  ///< reserved
  rcode response : 4;  ///< (response only)
  uint16_t qd_count;   ///< number of questions
  uint16_t an_count;   ///< number of answers
  uint16_t ns_count;   ///< number of authoritative answers
  uint16_t ar_count;   ///< number of additional answers
} header_t;
static_assert(12 == sizeof(header_t), "incorrectly packed");

/// using DNS compression semantics
/// (https://datatracker.ietf.org/doc/html/rfc1035#section-4.1.4)
typedef struct __attribute__((packed)) {
  uint16_t offset;  ///< packet relative offset to query name
  uint16_t type;
  uint16_t dns_class;
  uint32_t ttl;
  uint16_t addr_len;
  uint32_t ipv4_addr;
} compressed_answer;

/// using DNS compression semantics
/// (https://datatracker.ietf.org/doc/html/rfc1035#section-4.1.4)
constexpr uint16_t compressed_name_mask = 0XC000;

namespace rr {
enum type {
  A = 1,  ///< host address
};

enum dns_class {
  IN = 1,
};
}  // namespace rr
}  // namespace dns

static size_t sizeof_qname(const uint8_t* const qname)
{
  size_t cursor = 0;
  do {
    if (qname[cursor] == 0) {
      ESP_LOGD(WifiAccessPoint::TAG, "qname '%s' size: %d",
                qname, cursor + sizeof(uint8_t));
      return cursor + sizeof(uint8_t);
    }
    cursor += sizeof(uint8_t) + qname[cursor];
  } while(true);
}

void dns_service_task(void *) {
  // find out our IP info
  esp_netif_ip_info_t ip_info;
  esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"),
                        &ip_info);

  // create the listening socket
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  assert(sock > 0);
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(53 /* DNS PORT */);
  assert(0 == bind(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)));
  ESP_LOGI(WifiAccessPoint::TAG, "DNS started");

  int8_t request_count = 0;
  while (true) {
    static uint8_t buffer[256];
    struct sockaddr from;
    socklen_t from_sz = sizeof(from);
    const int sz = recvfrom(sock, buffer, sizeof(buffer), 0 /* flags (none) */,
                            &from, &from_sz);
    assert(sz >= 0);  // make sure our socket is still working
    if (sz <= sizeof(dns::header_t)) {
      ESP_LOGW(WifiAccessPoint::TAG, "dns request too small (ignoring)");
      continue;
    }

    // create reply (reusing the request buffer for the response)
    {
      auto header = reinterpret_cast<dns::header_t *>(buffer);
      if (header->op != dns::opcode::query) {
        ESP_LOGW(WifiAccessPoint::TAG, "invalid DNS opcode [%i] (ignoring)",
                header->op);
        continue;
      }
      // change the packet type to response
      header->qr = dns::type::response;
      size_t requestCursor = sizeof(dns::header_t);
      size_t responseCursor = sz;
      uint16_t an_count;
      size_t response_sz = sz;
      const auto qd_count = htons(header->qd_count);
      for (an_count = 0; an_count < qd_count; ++an_count) {
        ESP_LOGD(WifiAccessPoint::TAG, "dns request[%d] for '%s'",
                request_count, &buffer[requestCursor]);
        if ((responseCursor + sizeof(dns::compressed_answer)) > sizeof(buffer)) {
          ESP_LOGW(WifiAccessPoint::TAG,
                  "unable to respond to all queries (buffer too small)");
          // mark the response as truncated
          header->tc = true;
          break;
        }
        auto answer =
            reinterpret_cast<dns::compressed_answer *>(buffer + responseCursor);
        answer->offset = htons(dns::compressed_name_mask | requestCursor);
        answer->type = htons(dns::rr::type::A);
        answer->dns_class = htons(dns::rr::dns_class::IN);
        constexpr size_t TTL_sec = 1;
        answer->ttl = htonl(TTL_sec);
        answer->addr_len = htons(sizeof(ip_info.ip.addr));
        answer->ipv4_addr = ip_info.ip.addr;

        // update response cursor
        responseCursor += sizeof(dns::compressed_answer);
        response_sz += sizeof(dns::compressed_answer);

        // update request cursor
        const size_t qname_sz = sizeof_qname(buffer + requestCursor);
        requestCursor +=
            qname_sz + sizeof(uint16_t) /* qtype */ + sizeof(uint16_t) /* qclass */;
      }

      // send response
      ESP_LOGD(WifiAccessPoint::TAG, "dns response[%d] queries[%d] answers[%d]",
                request_count, qd_count, an_count);
      header->an_count = htons(an_count);
      sendto(sock, buffer, response_sz, 0 /* flags (none) */, &from, from_sz);

      ++request_count;
    }
  }

  // remove thread from scheduler
  vTaskDelete(nullptr);
}
