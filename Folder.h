//
// Created by Erik on 20.08.2017.
//


#pragma once

#include <forward_list>
#include <string>
#include "Functions.h"
#include "File.h"

using std::forward_list;
using std::string;

namespace Backup {
    class Folder {
    public:
        Folder(const string &rootPath, const string &relPath);
        string getRelPath();
        unsigned int getFileCount();
        unsigned int getFileProcessedCount();
    protected:
        forward_list<File> m_files;
        forward_list<Folder> m_folders;
        const string m_relPath;

        unsigned int m_filesProcessed;

        void backup(const string &rootSrc, const string &rootDest, const string &rootComp);
    private:
        void load(const string &path, const string &rootPath);
        void create(const string &path);
        void addFile(File f);
        void addFolder(Folder f);
    };
}