//
// Created by Erik on 20.08.2017.
//


#pragma once


#include <string>
#include <deque>
#include <sstream>
#include <iterator>
#include <Windows.h>
#include <forward_list>

namespace Backup {
    std::string findAbsolutePath(const std::string &AbsRoot, const std::string &relPath);
    std::forward_list<std::string> findFiles(const std::string &path);
    std::forward_list<std::string> findFolders(const std::string &path);
}
