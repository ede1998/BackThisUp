//
// Created by Erik on 20.08.2017.
//


#pragma once


#include <string>
#include <deque>
#include <sstream>
#include <iterator>
#include <Windows.h>
#include <shlwapi.h>
#include <forward_list>
#include <ctime>

namespace FilesystemFunctions {
    std::string findAbsolutePath(const std::string &AbsRoot, const std::string &relPath);
    std::forward_list<std::string> findFiles(const std::string &path);
    std::forward_list<std::string> findFolders(const std::string &path);
    bool isValidPath(const std::string &path);
}

namespace TimeFunctions {
    std::string timeToString(std::time_t time);
}
