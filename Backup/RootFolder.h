//
// Created by Erik on 20.08.2017.
//


#pragma once

#include "Folder.h"

namespace Backup {
    class RootFolder : public Folder {
    public:
        RootFolder(const std::string &absPathSrc, const std::string &absPathDest);
        RootFolder() noexcept;
        std::string getAbsPathSrc() const;
        std::string getAbsPathDest() const;
        void backup(const std::string &compPath = "");
    private:
        std::string m_AbsPathSrc;
        std::string m_AbsPathDest;
        bool m_empty;
    };
}


