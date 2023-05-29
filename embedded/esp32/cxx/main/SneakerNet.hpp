#include <atomic>

#include <esp_vfs_fat.h>

class SneakerNet {
public:
    SneakerNet();

    enum class State {
        INIT,
        SDCARD_FAILED,
        UNINITIALIZED,  /* passwordless admin access */
        OK
    };
    State getState();

private:
    std::atomic<State> state{State::INIT};

    sdmmc_card_t *card;
    bool mount_sdcard();
};