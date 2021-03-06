//
// Created by Erik on 20.08.2017.
//

#include "RootFolder.h"

namespace Backup {
    RootFolder::RootFolder(const std::string &absPathSrc, const std::string &absPathDest)
            : Folder((FilesystemFunctions::isValidPath(absPathSrc)) ? absPathSrc : throw std::invalid_argument("Source path is invalid"), "."),
              m_AbsPathDest((FilesystemFunctions::isValidPath(absPathDest)) ? absPathDest : throw std::invalid_argument("Destination path is invalid")),
              m_AbsPathSrc(absPathSrc), //already checked before calling Folder constructor
              m_empty(false) {

    }

    RootFolder::RootFolder() noexcept
            : m_empty(true) {

    }

    std::string RootFolder::getAbsPathSrc() const {
        return m_AbsPathSrc;
    }

    std::string RootFolder::getAbsPathDest() const {
        return m_AbsPathDest;
    }

    void RootFolder::backup(const std::string &compPath) {
        if (!m_empty)
            Folder::backup(m_AbsPathSrc, m_AbsPathDest, compPath);
    }
}