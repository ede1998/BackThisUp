//
// Created by Erik on 20.08.2017.
//


#pragma once


#include "Folder.h"

namespace Backup {
    class RootFolder : public Folder {
    public:
        RootFolder(const string &absPathSrc, const string &absPathDest);
        RootFolder() noexcept;
        string getAbsPathSrc() const;
        string getAbsPathDest() const;
        void backup(const string &compPath = "");
    private:
        string m_AbsPathSrc;
        string m_AbsPathDest;
        bool m_empty;
    };
}


