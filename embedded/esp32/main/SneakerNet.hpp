#pragma once
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <istream>

#include <esp_vfs_fat.h>
#include "sdkconfig.h"

class SneakerNet {
public:
    SneakerNet();

    enum class State {
        INIT,
        SDCARD_FAILED,
        PASSWORDLESS,   /* passwordless admin access */
        OK
    };
    State getState() { return state; }

    typedef std::string PublisherUuid;
    typedef std::string ContentUuid;
    typedef std::map<PublisherUuid, std::vector<ContentUuid>> Catalog;
    /// listing of content
    const Catalog catalog();
    /// read an eBook
    std::ifstream readCatalogItem(const std::string filename);

private:
    State state;

    sdmmc_card_t *card;
    const std::string MOUNT_PATH = "/sdcard";
    bool mount_sdcard();
};