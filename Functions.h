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
#include <fstream>
#include <map>
#include <functional>

namespace FilesystemFunctions {
    std::string findAbsolutePath(const std::string &AbsRoot, const std::string &relPath);
    std::forward_list<std::string> findFiles(const std::string &path);
    std::forward_list<std::string> findFolders(const std::string &path);
    bool isValidPath(const std::string &path);
    std::ifstream::pos_type getFileSize(std::ifstream &f);
    std::ifstream::pos_type getFileSize(const std::string &path);

    std::string loadFileToString(const std::string &filePath);
    bool saveStringToFile(const std::string &filePath, const std::string &content);

    void processFileByChunk(const std::string &path, unsigned int chunkSize, std::function<void (char *, const unsigned int)> processingFu);
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
    std::string combineString(std::string inputStr, const std::map<std::string, std::string> &args);
}

std::string exec(const char *cmd);