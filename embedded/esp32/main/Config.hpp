#include <vector>

class Config {
public:
    // loads configuration from file `sneakernet.cfg`
    Config();

    char controlled_size_percent;   /* any remaining space used for uncontrolled content */
    std::vector<Signature> publishers;

    typedef int days;
    struct {
        days uncontrolled_content;
        days controlled_content;
    } lifetimes;
};
