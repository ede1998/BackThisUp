//
// Created by Erik on 20.08.2017.
//


#pragma once


#include <string>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iterator>
#include "Functions.h"
#include "Logger.h"

using std::string;
namespace Backup {
    class File {
    public:
        explicit File(const string &relPath);
        string getRelPath();
        bool equals(const string &root1, const string &root2);
        void copy(const string &rootSrc, const string &rootDest);
    private:
        const string m_relPath;
    };
}