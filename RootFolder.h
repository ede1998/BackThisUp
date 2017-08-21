//
// Created by Erik on 20.08.2017.
//


#pragma once


#include "Folder.h"

namespace Backup {
    class RootFolder : public Folder {
    public:
        RootFolder(const string &absPathSrc, const string &absPathDest);
        string getAbsPathSrc();
        string getAbsPathDest();
        void backup(const string &compPath = "");
    private:
        const string m_AbsPathSrc;
        const string m_AbsPathDest;
    };
}


