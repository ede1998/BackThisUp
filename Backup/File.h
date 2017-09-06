//
// Created by Erik on 20.08.2017.
//


#pragma once


#include <string>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iterator>
#include "../Functions.h"
#include "../LoggingTools/Logger.h"

namespace Backup {
    class File {
    public:
        explicit File(const std::string &relPath);
        std::string getRelPath();
        bool equals(const std::string &root1, const std::string &root2);
        void copy(const std::string &rootSrc, const std::string &rootDest);
    private:
        const std::string m_relPath;
    };
}