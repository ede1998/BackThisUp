cmake_minimum_required(VERSION 3.8)
project(BackThisUp)

set(CMAKE_CXX_STANDARD 17)
add_subdirectory(LoggingTools)
add_subdirectory(Backup)
add_subdirectory(TrayIcon)
add_subdirectory(WebTools)

set(SOURCE_FILES main.cpp Functions.h Functions.cpp Definitions.h)
add_executable(BackThisUp ${SOURCE_FILES})

target_link_libraries(BackThisUp LoggingTools)
target_link_libraries(BackThisUp Backup)
target_link_libraries(BackThisUp TrayIcon)
target_link_libraries(BackThisUp WebTools)
target_link_libraries(BackThisUp shlwapi.lib)