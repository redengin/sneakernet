#pragma once

#include <filesystem>
#include <string>
#include <map>

class Catalog {
public:
    Catalog(const std::filesystem::path& path) : path(path) {};
    const std::filesystem::path path;

    /// @brief create an entry for each valid found file in catalog::`path`
    /// @post invalid files are deleted
    void init();

    /// key used for catalog entries
    typedef std::string Filename;

    /// add an Entry for a file
    /// @post if not able to be added, the file will be deleted
    bool add(const Filename& filename);

    typedef std::string sha256_t;
    typedef struct Entry {
        sha256_t sha256;
        bool sneakernetSigned;
    } Entry;
    /// @returns thread-safe map - won't invalidate thread iterators upon modification
    const std::map<Filename, Entry> getCatalog() {
        return catalog;
    }

private:
    /// thread-safe map - won't invalidate thread iterators upon modification
    std::map<Filename, Entry> catalog;

    /// create a Entry for an epub book
    /// @details filename must exist in `path`
    bool addEpub(const Filename& filename);

    /// create a Entry for a generic file
    /// @details filename must exist in `path`
    bool addFile(const Filename& filename);
};

