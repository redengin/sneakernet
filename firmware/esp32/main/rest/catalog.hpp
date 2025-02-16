#pragma once

#include "../WebServer.hpp"
#include "../Catalog.hpp"

namespace rest::catalog {
    static constexpr char TAG[] = "rest::catalog"; ///< ESP logging tag

    /// HTTP header field for timestamp
    /// @details ISO 8601 Z (aka UTC time)
    static constexpr char XFILETIMESTAMP[] = "X-timestamp";
    static constexpr size_t ISO8601_MAX_LENGTH = 27;

    static constexpr std::string_view uri_wildcard = "/api/catalog/*";
    void registerHandlers(WebServer& webserver, Catalog& catalog);

};