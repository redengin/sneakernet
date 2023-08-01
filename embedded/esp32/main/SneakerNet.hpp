#pragma once
#include <esp_vfs_fat.h>

#include "sneakernet/Catalog.hpp"

class SneakerNet {
public:
    SneakerNet();

    // librarian controlled content
    Catalog catalog;

private:
    void mount_sdcard();
    sdmmc_card_t *pCard;
};
