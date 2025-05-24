#pragma once

#include <string>

class WifiAccessPoint {
public:
    static constexpr char TAG[] = "WifiAccessPoint";    ///< ESP logging tag

    /// @brief Creates the Open Wifi Access Point
    WifiAccessPoint(const std::string& ssid);

    // provide system context with number of sockets created
    static constexpr int socketsUsed = 1;   // DNS handler

    /// @brief change the published SSID
    void setSsid(const std::string& ssid);

    /// @brief set the captive portal uri (DHCP 114 option)
    void setCaptivePortalUri(const std::string& uri);

private:
    char* captivePortalUri;
};
