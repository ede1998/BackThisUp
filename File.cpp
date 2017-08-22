//
// Created by Erik on 20.08.2017.
//


#include "File.h"

namespace {
    std::ifstream::pos_type getFileSize(std::ifstream &f) {
        std::ifstream::pos_type size = f.seekg(0, std::ifstream::end).tellg();
        f.seekg(0, std::ifstream::beg);
        return size;
    }
}
namespace Backup {
    File::File(const string &relPath)
            : m_relPath(relPath)
    {
    }

    string File::getRelPath() {
        return m_relPath;
    }

    bool File::equals(const string &root1, const string &root2) {
        if (root1.empty() || root2.empty())
            return false;

        using std::ifstream;
        std::ifstream in1(findAbsolutePath(root1, m_relPath), ifstream::binary),
                in2(findAbsolutePath(root2, m_relPath), ifstream::binary);

        if (!in1.is_open() || !in2.is_open())
            return false;
        ifstream::pos_type size1 = getFileSize(in1), size2 = getFileSize(in2);

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


    void File::copy(const string &rootSrc, const string &rootDest) {
        Logger &lg = Logger::getInstance();
        if (!CopyFile(findAbsolutePath(rootSrc, m_relPath).c_str(), findAbsolutePath(rootDest, m_relPath).c_str(), TRUE)) {
            lg.log("Could not copy file " + findAbsolutePath(rootSrc, m_relPath), LVL_ERROR);
        }
        else {
            lg.log("Copied file " + findAbsolutePath(rootSrc, m_relPath), LVL_INFO);
        }

    }
}