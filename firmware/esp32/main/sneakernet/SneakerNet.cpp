#include "SneakerNet.hpp"

#include <esp_app_desc.h>
#include <sdmmc_cmd.h>
#include <esp_log.h>
#include <filesystem>
#include <dirent.h> /* workaround for incomplete ESP32 filesystem support */
#include <sys/stat.h>
#include <utime.h>
#include <string.h>

static const char *TAG = "sneakernet";
constexpr char MOUNT_DIR[] = "/sdcard";
constexpr char INWORK_SUFFIX[] = ".inwork";

SneakerNet::SneakerNet()
{
    const esp_app_desc_t* const pDesc = esp_app_get_description();
    pVersion = pDesc->version;
    ESP_LOGI(TAG, "firmware version: %s", pVersion);

    // mount the sd card
    mount_sdcard();
    // cleanup contents and set system time per newest file
// FIXME ESP32 doesn't support directory_iterator (it's always empty)
    // for(const std::filesystem::directory_entry entry : std::filesystem::directory_iterator(MOUNT_DIR))
// Workaround - use C iterator
    DIR *dfd = opendir(MOUNT_DIR);
    const std::filesystem::path path = std::filesystem::path(MOUNT_DIR);
    for(struct dirent *pEntry = readdir(dfd); pEntry != nullptr; pEntry = readdir(dfd)) {
        // transmute pEntry into a directory entry
        std::filesystem::directory_entry entry(path/(pEntry->d_name));
        // ignore directories
        if(entry.is_directory()) continue;
        // remove aborted work
        if(entry.path().string().ends_with(INWORK_SUFFIX))
            std::filesystem::remove(entry);
    }
    closedir(dfd);
}

void SneakerNet::mount_sdcard()
{
// Pin mapping - avoiding JTAG pins (12, 13, 14, 15)
//--------------------------------------------------------------------------------
#define PIN_NUM_CS   2
#define PIN_NUM_MOSI 4
#define PIN_NUM_CLK  5
#define PIN_NUM_MISO 18
//--------------------------------------------------------------------------------
    ESP_LOGI(TAG, "Mounting sdcard filesystem");

    esp_log_level_set("vfs_fat_sdmmc", ESP_LOG_ERROR);
    const sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    constexpr int UNUSED = 0;
    constexpr int UNUSED_IO = -1;
    const spi_bus_config_t bus_cfg = {
        .mosi_io_num = PIN_NUM_MOSI,
        .miso_io_num = PIN_NUM_MISO,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = UNUSED_IO,
        .quadhd_io_num = UNUSED_IO,
        .data4_io_num = UNUSED_IO,
        .data5_io_num = UNUSED_IO,
        .data6_io_num = UNUSED_IO,
        .data7_io_num = UNUSED_IO,
        .max_transfer_sz = 0,   // 0: use full dma buffer support
        .flags = UNUSED,
        .isr_cpu_id = INTR_CPU_ID_AUTO,
        .intr_flags = UNUSED,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(static_cast<spi_host_device_t>(host.slot),
                                       &bus_cfg, SDSPI_DEFAULT_DMA));

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = static_cast<gpio_num_t>(PIN_NUM_CS);
    slot_config.host_id = static_cast<spi_host_device_t>(host.slot);
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = true,
        .max_files = 10,    // FIXME tune per RAM availability
        .allocation_unit_size = 512,    // required value for sdcards for wear leveling
        .disk_status_check_enable = false,
    };
    ESP_ERROR_CHECK(esp_vfs_fat_sdspi_mount(MOUNT_DIR,
                                            &host, &slot_config, &mount_config, &pCard));

    sdmmc_card_print_info(stdout, pCard);
}


std::vector<SneakerNet::content_t> SneakerNet::contents()
{
    std::vector<SneakerNet::content_t> ret;
// FIXME ESP32 doesn't support directory_iterator (it's always empty)
    // for(const std::filesystem::directory_entry entry : std::filesystem::directory_iterator(MOUNT_DIR))
// Workaround - use C iterator
    DIR *dfd = opendir(MOUNT_DIR);
    const std::filesystem::path path = std::filesystem::path(MOUNT_DIR);
    for(struct dirent *pEntry = readdir(dfd); pEntry != nullptr; pEntry = readdir(dfd)) {
        // transmute pEntry into a directory entry
        std::filesystem::directory_entry entry(path/(pEntry->d_name));
        if(entry.is_directory()) continue;
        if(entry.path().string().ends_with(INWORK_SUFFIX)) continue;
        struct stat statBuffer;
        if(0 != stat(entry.path().c_str(), &statBuffer))
        {
            ESP_LOGW(TAG, "Unable to stat %s [%s]",
                    entry.path().c_str(), strerror(errno));
        }
        ret.emplace_back(entry.path().filename(),
                         // entry.last_write_time(),
                         statBuffer.st_mtime,
                         entry.file_size()
        );
    }
    closedir(dfd);
    return ret;
}

static bool isValidContentName(const std::string& filename)
{
    // make sure it's only a name (no path)
    if(filename.find(std::filesystem::path::preferred_separator) != filename.npos) return false;
    return true;
}

std::ifstream SneakerNet::readContent(const std::string& filename)
{
    // validate filename
    if(isValidContentName(filename)) {
        const std::filesystem::path path = std::filesystem::path(MOUNT_DIR);
        return std::ifstream(path/filename, std::ios_base::in | std::ios_base::binary);
    }
    // else return a null stream
    return std::ifstream();
}

time_t SneakerNet::getFiletime(const std::string& filename)
{
    const std::filesystem::path path = std::filesystem::path(MOUNT_DIR);
    std::filesystem::directory_entry entry(path/filename);
    struct stat statBuffer;
    if(0 != stat(entry.path().c_str(), &statBuffer))
    {
        ESP_LOGW(TAG, "Unable to stat %s [%s]",
                entry.path().c_str(), strerror(errno));
    }
    return statBuffer.st_mtime;
}

void SneakerNet::removeContent(const std::string& filename)
{
    if(isValidContentName(filename)) {
        const std::filesystem::path path = std::filesystem::path(MOUNT_DIR);
        std::filesystem::remove(path/filename);
    }
}

off_t SneakerNet::delete_oldest_content()
{
    std::filesystem::directory_entry oldest;
    DIR *dfd = opendir(MOUNT_DIR);
    const std::filesystem::path path = std::filesystem::path(MOUNT_DIR);
    for(struct dirent *pEntry = readdir(dfd); pEntry != nullptr; pEntry = readdir(dfd)) {
        // transmute pEntry into a directory entry
        std::filesystem::directory_entry entry(path/(pEntry->d_name));
        if(entry.is_directory()) continue;
        if(entry.path().string().ends_with(INWORK_SUFFIX)) continue;
        else {
            // find oldest file
            if(false == oldest.exists())
                oldest = entry;
            else if(entry.last_write_time() < oldest.last_write_time())
                oldest = entry;
        }
    }
    closedir(dfd);

    if(oldest.exists())
    {
        off_t sz = oldest.file_size();
        std::filesystem::remove(oldest);
        return sz;
    }
    else return 0;
}

SneakerNet::InWorkContent SneakerNet::newContent(const std::string& filename, const size_t& file_size, const time_t& timestamp)
{
    // make room for new content (delete oldest files)
    uint64_t remaining;
    esp_vfs_fat_info(MOUNT_DIR, NULL, &remaining);
    while(remaining < file_size) {
        const off_t freed = delete_oldest_content();
        if(freed == 0)
            return SneakerNet::InWorkContent("/dev/null", 0);
        remaining -= freed;
    }

    return SneakerNet::InWorkContent(filename, timestamp);
}

SneakerNet::InWorkContent::InWorkContent(const std::string& filename, const time_t& timestamp)
    : filename(filename)
     ,timestamp(timestamp)
{
    const std::filesystem::path path = std::filesystem::path(MOUNT_DIR);
    ofs = std::ofstream(path/(filename + INWORK_SUFFIX),
                        std::ios_base::out | std::ios_base::binary);
}

bool SneakerNet::InWorkContent::write(const char buffer[], const size_t sz)
{
    ofs.write(buffer, sz);
    return ofs.good();
}

void SneakerNet::InWorkContent::done()
{
    ofs.close();
    const std::filesystem::path path = std::filesystem::path(MOUNT_DIR);
    // remove previous file if it exists
    if(std::filesystem::exists(path/filename))
        std::filesystem::remove(path/filename);
    std::filesystem::rename(path/(filename + INWORK_SUFFIX), path/filename);
    const struct utimbuf utimbuf{timestamp, timestamp};
    if(0 != utime((path/filename).c_str(), &utimbuf))
    {
        ESP_LOGW(TAG, "unable to change file time of %s : %s",
                (path/filename).c_str(), strerror(errno));

    }
}

SneakerNet::InWorkContent::~InWorkContent()
{
    // cancel incomplete transfer
    if(ofs.is_open())
    {
        ofs.close();
        const std::filesystem::path path = std::filesystem::path(MOUNT_DIR);
        std::filesystem::remove(path/(filename + INWORK_SUFFIX));
    }
}
