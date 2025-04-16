#pragma once

#include <filesystem>

class SdCard {
public:
    static constexpr char TAG[] = "SdCard"; ///< ESP logging tag

    SdCard();

    std::filesystem::path root() const
    { return std::filesystem::path(path); }

private:
    static constexpr char path[] = "/sdcard";

    // characterize read/write speed
    void characterize() const;
};
