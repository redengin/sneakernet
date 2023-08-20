# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(PROJECT_VER "0.0.1")

set(EXTRA_COMPONENT_DIRS
    $ENV{IDF_PATH}/components/json
)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32-sneakernet)

# minimize binary size
add_compile_options("-Os -fnortti -fdata-sections -ffunction-sections  -Wl,--gc-sections")
