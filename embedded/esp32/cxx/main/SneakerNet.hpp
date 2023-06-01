#pragma once
#include <string>
#include <map>
#include <vector>
#include <fstream>

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
    /// listing of epub content
    const Catalog catalog();
    /// read an eBook
    std::ifstream readEbook(const std::string uri /**< [IN] ebooks/<pub uuid>/<content uuid>**/);
#ifdef CONFIG_SNEAKERNET_FILES_SUPPORT
    typedef std::string Filename;
    typedef std::vector<Filename> FilesList;
    /// listing of files
    const FilesList files();
#endif    

private:
    State state;

    sdmmc_card_t *card;
    bool mount_sdcard();
};