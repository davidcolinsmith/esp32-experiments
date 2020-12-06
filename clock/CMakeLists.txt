cmake_minimum_required(VERSION 3.16.0)
include($ENV{HOME}/.platformio/packages/framework-espidf/tools/cmake/project.cmake)
project(clock)


set(EXT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/utils)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/main)
file(GLOB_RECURSE SOURCES ${SRC_DIR}/*.c)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
