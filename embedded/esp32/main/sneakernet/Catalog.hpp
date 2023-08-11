#pragma once

#include <filesystem>
#include <string>
#include <map>
#include <fstream>

#include "Sha256.hpp"

class Catalog {
public:
    Catalog(const std::filesystem::path& path) : path(path) {};

    /// @brief create an entry for each valid found file in catalog::`path`
    /// @post invalid files are deleted
    void init();

    /// a string without any path delimiters
    typedef std::string filename_t;
    typedef struct Entry {
        size_t size;
        sha256_t sha256;
        bool sneakernetSigned;
    } Entry;
    /// @returns thread-safe map - won't invalidate thread iterators upon modification
    const std::map<filename_t, Entry> items() { return catalog; }

    /// @return open file stream for stored file
    /// @details if the file isn't available, returns a null istream
    std::ifstream readItem(const filename_t&);

    class InWorkItem {
    public:
        /// open stream to write data to
        std::ofstream ofs;
        /// add the item to the catalog
        /// @details item will be validated per the librarian's config
        /// @retval true    accepted item
        /// @retval false   item rejected (see getMessages())
        bool add();
        const std::string getMessages() { return messages; }

        InWorkItem(Catalog* const, const filename_t);
        /// invalid work item
        InWorkItem();
        ~InWorkItem();
    private:
        Catalog* const catalog = nullptr;
        const filename_t filename;
        std::string messages;
    };
    InWorkItem newItem(const filename_t& filename, const size_t size);

    bool removeItem(const filename_t&);


private:
    const std::filesystem::path path;

    /// thread-safe map - won't invalidate thread iterators upon modification
    std::map<filename_t, Entry> catalog;

    /// add an Entry for a file
    /// @details will validate per librarian requirements
    /// @post if not able to be added, the file will be deleted
    bool add(const filename_t& filename);

    /// create a Entry for an epub book
    /// @details filename must exist in `path`
    bool addEpub(const filename_t& filename);

    /// create a Entry for a generic file
    /// @details filename must exist in `path`
    bool addFile(const filename_t& filename);
};

