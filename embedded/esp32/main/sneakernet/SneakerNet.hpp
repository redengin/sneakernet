#pragma once
#include <esp_vfs_fat.h>

#include "Catalog.hpp"

class SneakerNet {
public:
    SneakerNet();

    // librarian controlled content
    Catalog catalog;

    /// string of sha (with NULL terminator)
    char firmwareSha256[CONFIG_APP_RETRIEVE_LEN_ELF_SHA + 1];

private:
    void mount_sdcard();
    sdmmc_card_t *pCard;
};
