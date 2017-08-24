//
// Created by Erik on 20.08.2017.
//

#include "Folder.h"

using std::string;

namespace Backup {
    Folder::Folder(const string &rootPath, const string &relPath)
            : m_relPath(relPath)
    {
        load(FilesystemFunctions::findAbsolutePath(rootPath, relPath), rootPath);
    }

    string Folder::getRelPath() const {
        return m_relPath;
    }

    void Folder::addFile(File f) {
        m_files.push_front(f);
    }

    void Folder::addFolder(Folder f) {
        m_folders.push_front(f);
    }

    void Folder::load(const string &path, const string &rootPath) {
        std::forward_list<string> f = FilesystemFunctions::findFiles(path);
        std::forward_list<string> f1 = FilesystemFunctions::findFolders(path);

        if (std::find(std::begin(f), std::end(f), EXCLUDE_NAME) != std::end(f))
            exclude(path + R"(\)" + EXCLUDE_NAME,f, f1);

        for(const string &el: f)
            addFile(File(m_relPath + "\\" + el));
        for(const string &el: f1)
            addFolder(Folder(rootPath, m_relPath + "\\" + el));
    }

    void Folder::backup(const string &rootSrc, const string &rootDest, const string &rootComp) {
        create(FilesystemFunctions::findAbsolutePath(rootDest, m_relPath));
        for (File &f: m_files) {
            if (!f.equals(rootSrc, rootComp))
                f.copy(rootSrc, rootDest);
            m_filesProcessed++;
        }
        for (Folder &f: m_folders)
            f.backup(rootSrc, rootDest, rootComp);
    }

    void Folder::create(const string &path) {
        using namespace LoggingTools;
        Logger &lg = Logger::getInstance();
        if (!CreateDirectory(path.c_str(), nullptr)) {
            DWORD err = GetLastError();
            if (!((err == ERROR_ALREADY_EXISTS) || (err == ERROR_PATH_NOT_FOUND))) {
                lg.log("Could not create directory " + path, LVL_ERROR);
                lg.log("ErrorCode: " + err, LVL_NORMAL);
            }
        }
        else {
            lg.log("Created directory " + path, LVL_INFO);
        }
    }

    unsigned int Folder::getFileCount() const {
        auto tmp = (unsigned int) std::distance(std::begin(m_files), std::end(m_files));
        for (const Folder &f: m_folders)
            tmp += f.getFileCount();
        return tmp;
    }

    unsigned int Folder::getFileProcessedCount() const {
        unsigned int tmp = 0;
        for (const Folder &f: m_folders)
            tmp += f.getFileProcessedCount();
        return m_filesProcessed + tmp;
    }

    void Folder::exclude(const std::string &path, std::forward_list<string> &files, std::forward_list<string> &dirs) {
        //TODO: read file line for line, exclude named files
        //TODO: log these files
    }
}