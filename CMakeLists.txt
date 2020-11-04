cmake_minimum_required(VERSION 3.13)
project(native_addon_example)
set(CMAKE_CXX_STANDARD 11)

include_directories("~/.cache/node-gyp/14.7.0/include/node/")

set(SOURCE_FILES my-func.c wrapper.cpp)

add_executable(native_addon_example ${SOURCE_FILES})