#pragma once

#include <string>

class WifiAccessPoint {
public:
    static constexpr char TAG[] = "WifiAccessPoint";    ///< ESP logging tag

    // provide system context with number of sockets created
    static constexpr int socketsUsed = 1 /* DNS server */;

    /// @brief Creates the Open Wifi Captive Portal
    WifiAccessPoint(const std::string& ssid,
                    const std::string& captivePortalUri);

private:
    // captive portal uri must have a static placement
    const std::string captivePortalUri;
};
