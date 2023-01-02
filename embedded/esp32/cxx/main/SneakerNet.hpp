#include <atomic>

#include <esp_vfs_fat.h>

class SneakerNet {
public:
    SneakerNet();

    enum class Status {
        INIT,
        SDCARD_FAILED,
        UNINITIALIZED,  /* passwordless admin access */
        OK
    };
    Status getStatus();

private:
    std::atomic<Status> status{Status::INIT};

    sdmmc_card_t *card;
    bool mount_sdcard();

};