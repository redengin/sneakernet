#include "Catalog.hpp"

#include <fstream>
#include <esp_log.h>
static const char *TAG = "sneakernet-catalog";
#include <mbedtls/sha256.h>
#include <cstring>
#include <dirent.h>

static const std::string INWORK_SUFFIX = ".inwork";
static const sha256_t sha256(const std::filesystem::path& path);

void Catalog::init()
{
    ESP_LOGI(TAG, "init() initializing catalog");
    // ensure catalog directory
    std::error_code err;
    if(false == std::filesystem::is_directory(path, err)) {
        if(false == std::filesystem::create_directory(path, err)) {
            ESP_LOGE(TAG, "failed to create catalog directory");
            return;
        }
    }

    // FIXME appears ESP32 directory_iterator is always empty
    // for(const auto& entry : std::filesystem::directory_iterator(path)) {
    DIR *dfd = opendir(path.c_str());
    for(struct dirent *pEntry = readdir(dfd); pEntry != nullptr; pEntry = readdir(dfd)) {
        // transmute pEntry
        std::filesystem::directory_entry entry(path/(pEntry->d_name));
        if(entry.is_directory()) {
            std::filesystem::remove_all(entry, err);
            ESP_LOGW(TAG, "init() unable to remove directory %s", entry.path().c_str());
        }
        else {
            // drop any inwork files
            if(entry.path().string().ends_with(INWORK_SUFFIX)) {
                ESP_LOGI(TAG, "init() dropping inwork item '%s'", entry.path().c_str());
                std::filesystem::remove(entry.path(), err);
                if(err)
                    ESP_LOGW(TAG, "init() failed to remove inwork item '%s' [%s]",
                        entry.path().c_str(), err.message().c_str());
            }
            else {
                ESP_LOGI(TAG, "init() adding item '%s'", entry.path().c_str());
                add(entry.path().filename().string());
            }
        }

    }
    closedir(dfd);

    ESP_LOGI(TAG, "init() catalog size %d", catalog.size());
}

static bool isValidFileName(const Catalog::filename_t& filename) {
    // file name can not contain a delimeter
    return filename.find("/") == filename.npos;
}

std::ifstream Catalog::readItem(const filename_t& filename)
{
    // validate path
    if(isValidFileName(filename))
        return std::ifstream(path/filename, std::ios_base::in | std::ios_base::binary);

    return std::ifstream();
}


Catalog::InWorkItem Catalog::newItem(const std::string& filename, const size_t size)
{
    // validate path
    if(false == isValidFileName(filename)) {
        ESP_LOGI(TAG, "newItem() failed - illegal file name [%s]", filename.c_str());
        return InWorkItem();
    }

    if(catalog.contains(filename)) {
        ESP_LOGI(TAG, "newItem() failed - existing file of same name [%s]", filename.c_str());
        return InWorkItem();
    }

    // TODO clean up files to make room for new content size
    (void)size;

    return InWorkItem(this, filename);
}

bool Catalog::removeItem(const filename_t& filename)
{
    // validate path
    if(false == isValidFileName(filename))
        return false;

    auto found = catalog.find(filename);
    if(found == catalog.end())
        return false;

    catalog.erase(found);
    std::error_code err;
    std::filesystem::remove(path/filename, err);
    if(err)
        ESP_LOGW(TAG, "Catalog::removeItem unable to remove %s [err: %s]",
                      (path/filename).c_str(), err.message().c_str());
    return true;
}

Catalog::InWorkItem::InWorkItem() {};

Catalog::InWorkItem::InWorkItem(Catalog* const catalog, const filename_t filename)
    : catalog(catalog)
     ,filename(filename)
{
     ofs = std::ofstream(catalog->path/(filename + INWORK_SUFFIX), std::ios_base::out | std::ios_base::binary);
}

Catalog::InWorkItem::~InWorkItem()
{
    if(ofs.is_open())
        ofs.close();
    // FIXME ESP32 implementation of remove always throws (even if err is provided)
    // std::error_code err;
    // std::filesystem::remove(catalog->path/(filename + INWORK_SUFFIX), err);
};

bool Catalog::InWorkItem::add() {
    if(catalog == nullptr)
        return false;
    
    if(ofs.is_open())
        ofs.close();

    // rename the file
    std::error_code err;
    std::filesystem::rename(catalog->path/(filename + INWORK_SUFFIX), catalog->path/filename, err);
    if(err) {
        ESP_LOGE(TAG, "Catalog::InworkItem::add unable to rename %s [err: %s]",
                      (catalog->path/filename).c_str(), err.message().c_str());
        return false;
    }
    return catalog->add(filename);
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

bool Catalog::addFile(const std::string& filename) {
    std::error_code err;
    const size_t size = std::filesystem::file_size(path/filename, err);
    if( err || (size == 0) ) {
        ESP_LOGW(TAG, "addFile() unable to get size of file [%s]", err.message().c_str());
        return false;
    }

    Catalog::Entry entry = {
        .size = size,
        // currently unused, so don't use the compute time
        // .sha256 = sha256(path/filename),
        .sha256 = "",
        .sneakernetSigned = false,
    };

    // remove an entry that will be replaced
    catalog.erase(filename);

    // emplace().second is true if added
    return catalog.emplace(filename, entry).second;
}

bool Catalog::addEpub(const std::string& filename) {
    return addFile(filename);
    // // TODO validate per librarian settings

    // // remove an entry that will be replaced
    // catalog.erase(filename);

    // // FIXME get identifiers

    // // FIXME get sneakernet signature
    // // FIXME validate sneakernet signature

    // Catalog::Entry entry = {
    //     .sha256 = sha256(path/filename),
    //     .sneakernetSigned = false,
    // };

    // // emplace().second is true if added
    // return catalog.emplace(filename, entry).second;
}

const sha256_t sha256(const std::filesystem::path& path)
{
    sha256_t ret;

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
