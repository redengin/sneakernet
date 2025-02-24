#pragma once

#include "../WebServer.hpp"

#include <string_view>

namespace rest::firmware {
    static constexpr char TAG[] = "rest::firmware"; ///< ESP logging tag

    static constexpr std::string_view uri = "/api/firmware";
    void registerHandlers(WebServer& webserver);
};