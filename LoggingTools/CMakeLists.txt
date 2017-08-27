cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES Logger.h Logger.cpp)
add_library(LoggingTools ${SOURCE_FILES})