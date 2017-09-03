cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(curllib)
include_directories(curl)
link_directories(curl)
set(SOURCE_FILES CurlController.cpp CurlController.h)
add_library(curl ${SOURCE_FILES})
target_link_libraries(curl curl-d)