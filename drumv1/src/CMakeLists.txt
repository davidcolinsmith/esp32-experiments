# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(SRCS ${app_sources} "u8g2_esp32_hal.c" "display.c" "gate.c" "wifi.c" "osc.c"
    INCLUDE_DIRS ".")
