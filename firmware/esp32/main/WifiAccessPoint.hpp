#pragma once

#include <string>

class WifiAccessPoint {
public:
    static constexpr char TAG[] = "WifiAccessPoint";    ///< ESP logging tag

    // provide system context with number of sockets created
    static constexpr int socketsUsed = 1;

    /// @brief Creates the Open Wifi Access Point
    /// @post call publish()
    /// @param ssid 
    WifiAccessPoint();
};
