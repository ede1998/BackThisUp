//
// Created by Erik on 20.08.2017.
//


#include "File.h"

namespace Backup {
    File::File(const std::string &relPath)
            : m_relPath(relPath)
    {
    }

    std::string File::getRelPath() {
        return m_relPath;
    }

    bool File::equals(const std::string &root1, const std::string &root2) {
        using FilesystemFunctions::findAbsolutePath;
        if (root1.empty() || root2.empty())
            return false;

        using std::ifstream;
        std::ifstream in1(findAbsolutePath(root1, m_relPath), ifstream::binary),
                in2(findAbsolutePath(root2, m_relPath), ifstream::binary);

        if (!in1.is_open() || !in2.is_open())
            return false;
        ifstream::pos_type size1 = FilesystemFunctions::getFileSize(in1), size2 = FilesystemFunctions::getFileSize(in2);

        if (size1 != size2)
            return false;

        static const size_t BLOCKSIZE = 4096;
        size_t remaining = size1;

        while (remaining > 0) {
            char buffer1[BLOCKSIZE], buffer2[BLOCKSIZE];
            size_t size = std::min(BLOCKSIZE, remaining);

            in1.read(buffer1, size);
            in2.read(buffer2, size);

            if (0 != memcmp(buffer1, buffer2, size))
                return false;

            remaining -= size;
        }
        return true;
    }


    void File::copy(const std::string &rootSrc, const std::string &rootDest) {
        using namespace LoggingTools;
        using FilesystemFunctions::findAbsolutePath;

        Logger &lg = Logger::getInstance();
        if (!CopyFile(findAbsolutePath(rootSrc, m_relPath).c_str(), findAbsolutePath(rootDest, m_relPath).c_str(), TRUE)) {
            lg.log("Could not copy file " + findAbsolutePath(rootSrc, m_relPath), LVL_ERROR);
        }
        else {
            lg.log("Copied file " + findAbsolutePath(rootSrc, m_relPath), LVL_INFO);
        }

    }
}