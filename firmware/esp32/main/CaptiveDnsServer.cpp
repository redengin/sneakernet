#include <esp_log.h>
#include <sdkconfig.h>
// override LOG_LOCAL_LEVEL per project setting
#undef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL CONFIG_SNEAKERNET_LOG_LEVEL

#include <esp_task.h>
#include <socket.h>

#include "CaptiveDnsServer.hpp"

static void dns_service_task(void *);

CaptiveDnsServer::CaptiveDnsServer()
{
    assert(pdPASS == xTaskCreate(dns_service_task, "dns_service",
                                 2048 /* stack depth */,
                                 nullptr /* parameters (none) */,
                                 tskIDLE_PRIORITY /* priority */,
                                 nullptr /* handle (not used) */
                                 ));
}

void dns_service_task(void *)
{
    // create the listening socket
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    assert(sock > 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(53 /* DNS PORT */);
    assert(0 == bind(sock, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)));
    ESP_LOGI(CaptiveDnsServer::TAG, "DNS started");

    // service dns requests
    while (true)
    {
        // read the next request
        static uint8_t buffer[128];
        struct sockaddr_in6 from; // support largest address size (IPv6)
        socklen_t from_sz = sizeof(from);
        const int sz = recvfrom(sock, buffer, sizeof(buffer), 0 /* flags (none) */,
                                reinterpret_cast<struct sockaddr *>(&from), &from_sz);

        // create reply (reusing the request buffer for the response)
        auto header = reinterpret_cast<dns::header_t *>(buffer);
        header->qr = dns::header::type::response;
        size_t qdCursor = sizeof(dns::header_t);
        size_t anCursor = sz;
        size_t response_sz = sz;

        // append the answers
        size_t qd_count = ntohs(header->qd_count);
        size_t an_count;
        for (an_count = 0; an_count < qd_count; ++an_count)
        {
            ESP_LOGD(CaptiveDnsServer::TAG, "dns request[%d/%d] for '%s'",
                     an_count, qd_count, &buffer[qdCursor]);
            if ((anCursor + sizeof(dns::compressed_answer)) > sizeof(buffer))
            {
                ESP_LOGW(CaptiveDnsServer::TAG,
                         "unable to respond to all queries (buffer too small)");
                // mark the response as truncated
                header->tc = true;
                break;
            }
            // provide an answer
            auto answer =
                reinterpret_cast<dns::answer::compressed*>(buffer + anCursor);
            answer->offset = htons(dns::compressed_name_mask | anCursor);
            answer->type = htons(dns::rr::type::A);
            answer->dns_class = htons(dns::rr::dns_class::IN);
            constexpr size_t TTL_sec = 1;
            answer->ttl = htonl(TTL_sec);
            answer->ipv4_addr = ip_info.ip.addr;
            answer->addr_len = htons(sizeof(ip_info.ip.addr));
            // update response cursor
            responseCursor += sizeof(dns::compressed_answer);
            response_sz += sizeof(dns::compressed_answer);
            // update request cursor
            const size_t qname_sz = sizeof_qname(buffer + requestCursor);
            requestCursor +=
                qname_sz + sizeof(uint16_t) /* qtype */ + sizeof(uint16_t) /* qclass */;
        }

        // send response
        ESP_LOGD(CaptiveDnsServer::TAG, "dns response queries[%d] answers[%d]",
                 qd_count, an_count);
        header->an_count = htons(an_count);
        sendto(sock, buffer, response_sz, 0 /* flags (none) */,
               reinterpret_cast<struct sockaddr *>(&from), from_sz);
    }

    // task ended, remove thread from scheduler
    vTaskDelete(nullptr);
}
