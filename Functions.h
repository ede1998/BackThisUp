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
#include <vector>
#include <algorithm>

namespace FilesystemFunctions {
    std::string findAbsolutePath(const std::string &AbsRoot, const std::string &relPath);
    std::forward_list<std::string> findFiles(const std::string &path);
    std::forward_list<std::string> findFolders(const std::string &path);
    bool isValidPath(const std::string &path);
}

namespace TimeFunctions {
    std::string timeToString(std::time_t time);
    struct time {
        char minutes = 0;
        char seconds = 0;
        unsigned int hours = 0;
    };

    time secondsToTime(double seconds);
}

namespace StringFunctions {
    std::string parseParameters(const std::vector<std::string> &parameters, const std::string &argPrefix);
    bool doesParameterExist(const std::vector<std::string> &parameters, const std::string &argName);
}
