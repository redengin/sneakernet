#pragma once

#include <string>

class WifiAccessPoint {
public:
    static constexpr char TAG[] = "WifiAccessPoint";    ///< ESP logging tag

    /// @brief Creates the Open Wifi Access Point
    /// @post call publish()
    /// @param ssid 
    WifiAccessPoint(const std::string ssid = "SneakerNet");

    // provide system context with number of sockets created
    static constexpr int socketsUsed = 1;

    /// @brief change the published SSID
    void setSsid(const std::string ssid);

private:
    std::string ssid;
};
