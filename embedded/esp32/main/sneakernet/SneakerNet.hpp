#pragma once
#include <esp_vfs_fat.h>

#include "Sha256.hpp"
#include "Catalog.hpp"

class SneakerNet {
public:
    SneakerNet();

    // librarian controlled content
    Catalog catalog;

    const sha256_t firmwareSha256;

private:
    void mount_sdcard();
    sdmmc_card_t *pCard;
};
