//
// Created by Erik on 20.08.2017.
//

#include "RootFolder.h"
namespace Backup {
    RootFolder::RootFolder(const string &absPathSrc, const string &absPathDest)
            : Folder(absPathSrc, "."),
              m_AbsPathDest(absPathDest),
              m_AbsPathSrc(absPathSrc) {

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