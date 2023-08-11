#pragma once
#include <esp_vfs_fat.h>
#include <esp_ota_ops.h>

#include "sneakernet/Catalog.hpp"

class SneakerNet {
public:
    SneakerNet();

    // librarian controlled content
    Catalog catalog;

    // const esp_app_desc_t firmwareInfo;

private:
    void mount_sdcard();
    sdmmc_card_t *pCard;
};
