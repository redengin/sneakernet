#pragma once
#include <esp_vfs_fat.h>

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

class SneakerNet {
public:
    SneakerNet();

    typedef struct {
        std::string filename;
        // std::filesystem::file_time_type timestamp;
        time_t timestamp;
        size_t size;
    } content_t;
    std::vector<content_t> contents();

    /// @return open file stream for stored file
    /// @details if the file isn't available, returns a null istream
    std::ifstream readContent(const std::string& filename);

    void removeContent(const std::string& filename);

    class InWorkContent {
    public:
        InWorkContent(const std::string& filename, const time_t& timestamp);
        bool write(const char buffer[], const size_t sz);
        void done();
        ~InWorkContent();
    private:
        const std::string& filename;
        const time_t timestamp;
        std::ofstream ofs;
    };
    /// start a new content transfer
    InWorkContent newContent(const std::string& filename, const size_t& file_size, const time_t& timestamp);

    /// firmware version
    const char* pVersion;

private:
    sdmmc_card_t *pCard;
    void mount_sdcard();
    off_t delete_oldest_content();
};
