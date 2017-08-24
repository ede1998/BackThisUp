//
// Created by Erik on 20.08.2017.
//


#pragma once

#include <forward_list>
#include <string>
#include "Functions.h"
#include "File.h"
#include "Definitions.h"

namespace Backup {
    class Folder {
    public:
        Folder(const std::string &rootPath, const std::string &relPath);
        std::string getRelPath() const;
        unsigned int getFileCount() const;
        unsigned int getFileProcessedCount() const;
        void setIgnoreExcludes(bool ignoreExcludes);
    protected:
        std::forward_list<File> m_files;
        std::forward_list<Folder> m_folders;
        std::string m_relPath;

        unsigned int m_filesProcessed{0};

        Folder() = default;
        void backup(const std::string &rootSrc, const std::string &rootDest, const std::string &rootComp);
    private:
        bool m_doNotIgnore = false;
        void load(const std::string &path, const std::string &rootPath);
        void create(const std::string &path);
        void addFile(File f);
        void addFolder(Folder f);
        void exclude(const std::string &path, std::forward_list<std::string> &files, std::forward_list<std::string> &dirs);
    };
}