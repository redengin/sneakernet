#include "Catalog.hpp"

#include <fstream>
#include <esp_log.h>
static const char *TAG = "sneakernet-catalog";
#include <mbedtls/sha256.h>
#include <cstring>

void Catalog::init()
{
    for(auto const& entry : std::filesystem::directory_iterator{path}) {
        // don't allow subdirectories
        if(entry.is_directory()) std::filesystem::remove_all(entry);
        else {
            // drop any inwork files
            if(entry.path().string().ends_with(".inwork")) {
                std::error_code err;
                std::filesystem::remove(entry.path(), err);
                if(err)
                    ESP_LOGW(TAG, "init() failed to remove inwork item '%s' [%s]",
                        entry.path().string().c_str(), err.message().c_str());
            }
            else
                add(entry.path().filename().string());
        }
    }
}

bool Catalog::add(const std::string& filename)
{
    bool ret = true;

    if(filename.ends_with(".epub"))
        ret = addEpub(filename);
    else
        ret = addFile(filename);

    if(ret == false) {
        ESP_LOGI(TAG, "add() removing invalid catalog item '%s'", filename.c_str());
        std::error_code err;
        std::filesystem::remove(path / filename, err);
        if(err)
            ESP_LOGW(TAG, "add() failed to remove invalid catalog item '%s' [%s]",
                filename.c_str(), err.message().c_str());
    }
    else
        ESP_LOGI(TAG, "add() added catalog item '%s'", filename.c_str());

    return ret;
}

const Catalog::sha256_t sha256(const std::filesystem::path& path)
{
    Catalog::sha256_t ret;

    auto ifs = std::ifstream(path, std::ios_base::in | std::ios_base::binary);
    if(ifs.bad()) {
        ESP_LOGE(TAG, "sha256() unable to open file '%s' [%s]", path.c_str(), strerror(errno));
        return ret;
    }

    int err = 0;
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    err = mbedtls_sha256_starts(&ctx, false /* use sha256 rather than sha224 */);
    if(err != 0)
        ESP_LOGE(TAG, "sha256 unable to mbedtls_sha256_starts() [error: %X]", err);

    char buffer[64];
    while(false == ifs.eof()) {
        ifs.read(buffer, sizeof(buffer));
        err = mbedtls_sha256_update(&ctx, reinterpret_cast<unsigned char*>(buffer), ifs.gcount());
        if(err != 0) {
            ESP_LOGE(TAG, "sha256 unable to mbedtls_sha256_update() [error: %X]", err);
            break;    
        }
    }
    ifs.close();
    if(err == 0) {
        uint8_t sha256[32];
        err = mbedtls_sha256_finish(&ctx, sha256);
        if(err == 0)
            for(size_t i=0; i < sizeof(sha256); ++i) {
                char s[3];
                std::snprintf(s, sizeof(s), "%X", sha256[i]);
                ret += s;
            }
    }

    return ret;
}

bool Catalog::addFile(const std::string& filename) {
    // remove an entry that will be replaced
    catalog.erase(filename);

    Catalog::Entry entry = {
        .sha256 = sha256(path/filename),
        .sneakernetSigned = false,
    };

    // emplace().second is true if added
    return catalog.emplace(filename, entry).second;
}

bool Catalog::addEpub(const std::string& filename) {
    // TODO validate per librarian settings

    // remove an entry that will be replaced
    catalog.erase(filename);

    // FIXME get identifiers

    // FIXME get sneakernet signature
    // FIXME validate sneakernet signature

    Catalog::Entry entry = {
        .sha256 = sha256(path/filename),
        .sneakernetSigned = false,
    };

    // emplace().second is true if added
    return catalog.emplace(filename, entry).second;
}

