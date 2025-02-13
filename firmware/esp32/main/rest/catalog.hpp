#pragma once

#include "../WebServer.hpp"
#include "../Catalog.hpp"

namespace rest::catalog {
    static constexpr char TAG[] = "rest::catalog"; ///< ESP logging tag

    static constexpr std::string_view uri_wildcard = "/api/catalog/*";
    void registerHandlers(WebServer& webserver, Catalog& catalog);

};