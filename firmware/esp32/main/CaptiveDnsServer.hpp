#pragma once

class CaptiveDnsServer
{

public:
    static constexpr char TAG[] = "CaptiveDnsServer"; ///< ESP logging tag

    CaptiveDnsServer();

private:
};

namespace dns
{
#ifndef __GNUC__
#error requires GCC bit field packing.
#endif
#ifdef BITS_BIG_ENDIAN
#error requires bit fields to be packed little-endian (LSB first)
#endif
    namespace header
    {
        enum type
        {
            request = 0,
            response = 1,
        };

        enum opcode
        {
            query = 0,
            iquery = 1, ///< inverse query
            status = 2, ///< server status
        };

        enum rcode
        {
            ok = 0,
            query_err = 1,    ///< query format error
            server_err = 2,   ///< internal issue
            name_err = 3,     ///< name not recognized
            request_fail = 4, ///< request not supported
            ignored = 5,      ///< service ignored the request
        };
    }

    typedef struct __attribute__((packed))
    {
        uint16_t id; ///< unique id
        header::type qr : 1;
        header::opcode op : 4;
        bool aa : 1;                ///< authoritative result (response only)
        bool tc : 1;                ///< truncated message (response only)
        bool rd : 1;                ///< recursion desired
        bool ra : 1;                ///< recursion available (response only)
        uint8_t UNUSED : 3;         ///< reserved
        header::rcode response : 4; ///< (response only)
        uint16_t qd_count;          ///< number of questions
        uint16_t an_count;          ///< number of answers
        uint16_t ns_count;          ///< number of authoritative answers
        uint16_t ar_count;          ///< number of additional answers
    } header_t;
    static_assert(12 == sizeof(header_t), "incorrectly packed");

    namespace answer {
        /// using DNS compression semantics
        /// (https://datatracker.ietf.org/doc/html/rfc1035#section-4.1.4)
        typedef struct __attribute__((packed))
        {
            uint16_t offset;
            uint16_t type;
            uint16_t dns_class;
            uint32_t ttl;
            uint16_t addr_len;
            uint32_t ipv4_addr;
        } compressed;

        /// using DNS compression semantics
        /// (https://datatracker.ietf.org/doc/html/rfc1035#section-4.1.4)
        constexpr uint16_t compressed_name_mask = 0XC000;
    }
}
