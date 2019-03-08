//
// Created by Erik on 14.12.2017.
//


#pragma once

#include <vector>
#include <threads/mingw.thread.h>
#include <queue>
#include "Folder.h"
#include "RootFolder.h"

namespace Backup {
    class Manager {
    public:
        Manager(const std::string &absPathSrc, const std::string &absPathDest);
        void setTotalTreads(unsigned int nr);
        void init();
        void backup(const std::string &compPath = "");
    private:
        RootFolder m_rf;
        unsigned int m_totalThreads;
        std::queue<Folder*> m_tasks;
        std::forward_list<Folder*> m_folders;
        std::vector<std::thread> m_threads;
    };
}


