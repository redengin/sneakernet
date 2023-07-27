#pragma once

#include <filesystem>
#include <string>
#include <map>

class Catalog {
public:
    Catalog(const std::filesystem::path& path) : path(path) {};
    const std::filesystem::path path;

    /// @brief create an entry for each valid found file in `path`
    /// @post invalid files are deleted
    void init();

    /// key used for catalog entries
    typedef std::string CatalogFilename;
    /// create a CatalogEntry for a file
    /// @details filename must exist in `path`
    /// @post if not able to be added, the file will be deleted
    bool add(const CatalogFilename& filename);

private:
    typedef struct Sha256 {
        Sha256(const std::filesystem::path& path);
        unsigned char sha256[32];
        std::string toString() const;
    } Sha256;
    typedef struct CatalogEntry {
        const Sha256 sha256;
        bool sneakernetSigned;
    } CatalogEntry;
    /// thread-safe map - won't invalidate thread iterators upon modification
    std::map<CatalogFilename, CatalogEntry> catalog;

    /// create a CatalogEntry for an epub book
    /// @details filename must exist in `path`
    bool addEpub(const CatalogFilename& filename);

    /// create a CatalogEntry for a generic file
    /// @details filename must exist in `path`
    bool addFile(const CatalogFilename& filename);
};

