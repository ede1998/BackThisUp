//
// Created by Erik on 20.08.2017.
//

#include "RootFolder.h"

namespace Backup {
    RootFolder::RootFolder(const string &absPathSrc, const string &absPathDest)
            : Folder(absPathSrc, "."),
              m_AbsPathDest((FilesystemFunctions::isValidPath(absPathDest))?absPathDest:throw std::invalid_argument("Destionation path is invalid")),
              m_AbsPathSrc((FilesystemFunctions::isValidPath(absPathSrc))?absPathSrc:throw std::invalid_argument("Source path is invalid"))
    {

    }

    string RootFolder::getAbsPathSrc() {
        return m_AbsPathSrc;
    }

    string RootFolder::getAbsPathDest() {
        return m_AbsPathDest;
    }

    void RootFolder::backup(const string &compPath) {
        Folder::backup(m_AbsPathSrc, m_AbsPathDest, compPath);
    }
}