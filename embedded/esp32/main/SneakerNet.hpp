#pragma once
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <istream>

#include <esp_vfs_fat.h>
#include "sdkconfig.h"

#include "sneakernet/Catalog.hpp"

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

    static const std::string CATALOG_DIR;

    /// read an catalog item
    std::ifstream readCatalogItem(
        const std::string& path  ///< CATALOG_DIR/<filename>
    );

    class NewItem {
    public:
        NewItem() :path() {};
        NewItem(const std::string& path) :path(path) {}
        bool isBad() const { return path.empty(); }

        std::ofstream getOfstream();
        std::string getInworkPath() const;
    private:
        const std::string path;
    };
    /// create a new catalog item
    /// @post call addCatalogItem to validate/add the item
    NewItem createNewCatalogItem(
        const std::string& path,    ///< CATALOG_DIR/<filename>
        const size_t size           ///< size of content in bytes
    );
    enum AddNewCatalogItemStatus {
        OK,
        FAILED, ///< internal server error
    };
    /// add the new catalog item
    /// @pre the ofstream must be closed
    AddNewCatalogItemStatus addNewCatalogItem(const NewItem&);

private:
    State state;
    Catalog catalog;

    sdmmc_card_t *card;
    static const std::string MOUNT_PATH;
    bool mount_sdcard();

    bool isValidContentPath(const std::string& path) const;
    static const std::string CATALOG_NEW_ITEM_SUFFIX;
    AddNewCatalogItemStatus validateNewCatalogItem(const NewItem&);
};
