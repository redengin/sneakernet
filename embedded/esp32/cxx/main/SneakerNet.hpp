#pragma once
#include <atomic>
#include <string>
#include <map>

#include <esp_vfs_fat.h>

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
    typedef std::map<PublisherUuid, ContentUuid> Catalog;
    /// listing of content
    const Catalog catalog();

private:
    State state;

    sdmmc_card_t *card;
    bool mount_sdcard();
};