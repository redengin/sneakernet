#pragma once

#include "../WebServer.hpp"

namespace rest::firmware {
    static constexpr char TAG[] = "rest::firmware"; ///< ESP logging tag

    void registerHandlers(WebServer& webserver);
};