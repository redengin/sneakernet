#pragma once
#include <esp_vfs_fat.h>

#include <string>
#include <vector>
#include <fstream>

class SneakerNet {
public:
    SneakerNet();

    typedef struct {
        std::string filename;
        size_t size;
    } content_t;
    std::vector<content_t> contents();

    /// @return open file stream for stored file
    /// @details if the file isn't available, returns a null istream
    std::ifstream readContent(const std::string& filename);

    void removeContent(const std::string& filename);

    class InWorkContent {
    public:
        InWorkContent(const std::string& filename);
        bool write(const char buffer[], const size_t sz);
        void done();
        ~InWorkContent();
    private:
        const std::string& filename;
        std::ofstream ofs;
    };
    InWorkContent addContent(const std::string& filename, const size_t file_size);

    /// firmware version
    const char* pVersion;

private:
    sdmmc_card_t *pCard;
    void mount_sdcard();
};
