cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES TrayIcon.cpp TrayIcon.h)
add_library(TrayIcon ${SOURCE_FILES})
