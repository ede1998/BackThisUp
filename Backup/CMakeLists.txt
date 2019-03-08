cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES File.cpp File.h Folder.cpp Folder.h RootFolder.cpp RootFolder.h Manager.cpp Manager.h)
add_library(Backup ${SOURCE_FILES})