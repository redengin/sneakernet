#include "Catalog.hpp"

#include <fstream>
#include <esp_log.h>
static const char *TAG = "sneakernet-catalog";
#include <mbedtls/sha256.h>

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


bool Catalog::addEpub(const std::string& filename) {
    // TODO validate per librarian settings

    // remove an entry that will be replaced
    catalog.erase(filename);

    // FIXME not implemented

    return false;
}

bool Catalog::addFile(const std::string& filename) {
    // remove an entry that will be replaced
    catalog.erase(filename);

    Sha256 sha256 = Sha256(path/filename);

    Catalog::CatalogEntry entry = {
        .sha256 = sha256,
        .sneakernetSigned = false,
    };

    // emplace().second is true if added
    return catalog.emplace(filename, entry).second;
}

Catalog::Sha256::Sha256(const std::filesystem::path& path) {
    auto ifs = std::ifstream(path, std::ios_base::in | std::ios_base::binary);
    if(ifs.bad()) {
        ESP_LOGE(TAG, "sha256() unable to open file '%s'", path.c_str());
        return;
    }

    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, false /* use sha256 rather than sha224 */);

    char buffer[64];
    while(false == ifs.read(buffer, sizeof(buffer)).eof())
        // FIXME ilen should be equal to actual fill of buffer
        mbedtls_sha256_update(&ctx, reinterpret_cast<unsigned char*>(buffer), sizeof(buffer));

    // while( ifs.read(buffer, sizeofBuffer)
    // unsigned char data[1];
    // size_t data_sz = 1;

    ifs.close();
    mbedtls_sha256_finish(&ctx, sha256);
}
