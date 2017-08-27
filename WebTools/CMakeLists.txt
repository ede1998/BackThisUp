cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
add_definitions(-D_WIN32_WINNT=0x501)

set(SOURCE_FILES OneDriveConnector.cpp OneDriveConnector.h Parser.cpp Parser.h ServerSocket.cpp ServerSocket.h pw.h)
add_library(WebTools ${SOURCE_FILES})
target_link_libraries(WebTools Ws2_32.lib)