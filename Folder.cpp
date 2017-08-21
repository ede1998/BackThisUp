//
// Created by Erik on 20.08.2017.
//

#include "Folder.h"

namespace Backup {
    Folder::Folder(const string &rootPath, const string &relPath)
            : m_relPath(relPath)
    {
        load(findAbsolutePath(rootPath, relPath), rootPath);
    }

    string Folder::getRelPath() {
        return m_relPath;
    }

    void Folder::addFile(File f) {
        m_files.push_front(f);
    }

    void Folder::addFolder(Folder f) {
        m_folders.push_front(f);
    }

    void Folder::load(const string &path, const string &rootPath) {
        std::forward_list<string> f = Backup::findFiles(path);
        for(const string &el: f)
            addFile(File(m_relPath + "\\" + el));
        f.clear();
        f = Backup::findFolders(path);
        for(const string &el: f)
            addFolder(Folder(rootPath, m_relPath + "\\" + el));
    }

    void Folder::backup(const string &rootSrc, const string &rootDest, const string &rootComp) {
        create(findAbsolutePath(rootDest, m_relPath));
        for (File f: m_files)
            if (!f.equals(rootSrc, rootComp))
                f.copy(rootSrc, rootDest);
        for (Folder f: m_folders)
            f.backup(rootSrc, rootDest, rootComp);
    }

    void Folder::create(const string &path) {
        Logger * lg = Logger::getInstance();
        if (!CreateDirectory(path.c_str(), nullptr)) {
            DWORD err = GetLastError();
            if (!((err == ERROR_ALREADY_EXISTS) || (err == ERROR_PATH_NOT_FOUND))) {
                lg->log("Could not create directory " + path, LVL_ERROR);
                lg->log("ErrorCode: " + err, LVL_NORMAL);
            }
        }
        else {
            lg->log("Created directory " + path, LVL_INFO);
        }
    }

    unsigned int Folder::getFileCount() {
        auto tmp = (unsigned int) std::distance(std::begin(m_files), std::end(m_files));
        for (Folder f: m_folders)
            tmp += f.getFileCount();
        return tmp;
    }
}