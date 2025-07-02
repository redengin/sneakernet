#pragma once

#include "../WebServer.hpp"
#include "../Catalog.hpp"

namespace rest::catalog {
    static constexpr char TAG[] = "rest::catalog"; ///< ESP logging tag

    static constexpr std::string_view uri_sync = "/api/catalog.content";
    static constexpr std::string_view uri_content_wildcard = "/api/catalog/*";
    static constexpr std::string_view uri_title_wildcard = "/api/catalog.title/*";
    static constexpr std::string_view uri_icon_wildcard = "/api/catalog.icon/*";
    void registerHandlers(WebServer& webserver, Catalog& catalog);

    /// HTTP header field for timestamp
    /// @details ISO 8601 Z (aka UTC time)
    static constexpr char XFILETIMESTAMP[] = "X-Timestamp";
    static constexpr size_t ISO8601_MAX_LENGTH = 27;
};