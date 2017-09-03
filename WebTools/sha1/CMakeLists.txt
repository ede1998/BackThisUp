cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES sha1.cpp sha1.hpp)
add_library(sha1 ${SOURCE_FILES})