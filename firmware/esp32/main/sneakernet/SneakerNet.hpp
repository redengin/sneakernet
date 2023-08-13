#pragma once
#include <esp_vfs_fat.h>

#include "Catalog.hpp"

class SneakerNet {
public:
    SneakerNet();

    // librarian controlled content
    Catalog catalog;

    /// firmware version
    const char* pVersion;

private:
    void mount_sdcard();
    sdmmc_card_t *pCard;
};
