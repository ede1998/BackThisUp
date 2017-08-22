#include "Functions.h"

using std::deque;
using std::string;

namespace {

    template <char>
    class StringWithDelimiter: public string {};

    template<char delimiter>
    std::istream& operator>>(std::istream& is, StringWithDelimiter<delimiter> &output) {
        std::getline(is, output, delimiter);
        return is;
    }
    deque<string> split(const string &path) {
        std::istringstream iss(path);
        deque<string> results( (std::istream_iterator<StringWithDelimiter<'\\'>> (iss)),
                               std::istream_iterator< StringWithDelimiter<'\\'> > () );
        return results;
    }
}

namespace Backup {
    string findAbsolutePath(const string &absRoot, const string &relPath) {
        string path;
        deque<string> abs = split(absRoot);
        deque<string> rel = split(relPath);
        for (const string &el: rel) {
            if (el == "..") {
                abs.pop_back();
                if (abs.empty())
                    return "";
            }
            else if (el != ".")
                abs.push_back(el);
        }
        for (const string &el: abs) {
            path.append(el);
            path.append("\\");
        }
        path.pop_back();
        return path;
    }

    std::forward_list<string> findFiles(const std::string &path) {
        std::forward_list<string> files;
        string search_path = path + "\\*";
        WIN32_FIND_DATA fd{};
        HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
        if(hFind != INVALID_HANDLE_VALUE) {
            do {
                // read all (real) files in current folder
                if(! (bool)(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                    string tmp = fd.cFileName;
                    files.push_front(tmp);
                }
            }while(FindNextFile(hFind, &fd));
            FindClose(hFind);
        }
        return files;
    }

    std::forward_list<string> findFolders(const std::string &path) {
        std::forward_list<string> folders;
        string search_path = path + "\\*";
        WIN32_FIND_DATA fd{};
        HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
        if(hFind != INVALID_HANDLE_VALUE) {
            do {
                // read all (real) subfolders in current folder
                // skip default folder . and ..
                if( (bool)(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) {
                    string tmp = fd.cFileName;
                    if ((tmp != ".") && (tmp != ".."))
                        folders.push_front(tmp);
                }
            }while(FindNextFile(hFind, &fd));
            FindClose(hFind);
        }
        return folders;
    }
}