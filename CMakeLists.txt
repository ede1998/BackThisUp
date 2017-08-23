cmake_minimum_required(VERSION 3.8)
project(BackThisUp)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp File.cpp File.h Folder.cpp Folder.h RootFolder.cpp RootFolder.h Functions.h Functions.cpp Logger.cpp Logger.h TrayIcon/resource.h TrayIcon/resource.rc TrayIcon/TrayIcon.cpp TrayIcon/TrayIcon.h)
add_executable(BackThisUp ${SOURCE_FILES})

target_link_libraries(BackThisUp shlwapi.lib)