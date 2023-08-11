#pragma once

#include <string>

typedef std::string sha256_t;

namespace Sha256 {
    sha256_t fromBytes(const uint8_t* const bytes);
}
