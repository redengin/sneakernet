#pragma once

#include <string>

class WifiAccessPoint {
public:
    static constexpr char TAG[] = "WifiAccessPoint";    ///< ESP logging tag

    /// @brief Creates the Open Wifi Access Point
    /// @post call publish()
    /// @param ssid 
    WifiAccessPoint(const std::string ssid = "Little Free Library");

    /// @brief change the published SSID
    void setSsid(const std::string ssid);

private:
    std::string ssid;
};
