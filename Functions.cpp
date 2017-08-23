
#include "Functions.h"

using std::deque;
using std::string;

namespace {

    template<char>
    class StringWithDelimiter : public string {
    };

    template<char delimiter>
    std::istream &operator>>(std::istream &is, StringWithDelimiter<delimiter> &output) {
        std::getline(is, output, delimiter);
        return is;
    }

    deque<string> split(const string &path) {
        std::istringstream iss(path);
        deque<string> results((std::istream_iterator<StringWithDelimiter<'\\'>>(iss)),
                              std::istream_iterator<StringWithDelimiter<'\\'> >());
        return results;
    }
}

namespace FilesystemFunctions {
    string findAbsolutePath(const string &absRoot, const string &relPath) {
        string path;
        deque<string> abs = split(absRoot);
        deque<string> rel = split(relPath);
        for (const string &el: rel) {
            if (el == "..") {
                abs.pop_back();
                if (abs.empty())
                    return "";
            } else if (el != ".")
                abs.push_back(el);
        }
        for (const string &el: abs) {
            path.append(el);
            path.append("\\");
        }
        path.pop_back();
        return path;
    }

    std::forward_list<string> findFiles(const string &path) {
        std::forward_list<string> files;
        string search_path = path + "\\*";
        WIN32_FIND_DATA fd{};
        HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                // read all (real) files in current folder
                if (!(bool) (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    string tmp = fd.cFileName;
                    files.push_front(tmp);
                }
            } while (FindNextFile(hFind, &fd));
            FindClose(hFind);
        }
        return files;
    }

    std::forward_list<string> findFolders(const string &path) {
        std::forward_list<string> folders;
        string search_path = path + "\\*";
        WIN32_FIND_DATA fd{};
        HANDLE hFind = FindFirstFile(search_path.c_str(), &fd);
        if (hFind != INVALID_HANDLE_VALUE) {
            do {
                // read all (real) subfolders in current folder
                // skip default folder . and ..
                if ((bool) (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                    string tmp = fd.cFileName;
                    if ((tmp != ".") && (tmp != ".."))
                        folders.push_front(tmp);
                }
            } while (FindNextFile(hFind, &fd));
            FindClose(hFind);
        }
        return folders;
    }

    bool isValidPath(const string &path) {
        if (path.length() >= MAX_PATH)
            return false;
        if (path.length() == 0)
            return false;
        return PathFileExists(path.c_str()) == TRUE;
    }
}

namespace TimeFunctions {
    string timeToString(std::time_t time) {
        string res = ctime(&time);
        res.erase(res.length() - 1);
        return res;
    }

    time secondsToTime(double seconds) {
        time t{};
        time_t sec = (int) (seconds);
        if (sec >= 3600) {
            t.hours = (unsigned int) (sec % 3600);
            sec -= 3600*t.hours;
        }
        if (sec >=60) {
            t.minutes = (char) (sec % 60);
            sec -= 60*t.minutes;
        }
        t.seconds = (char) (sec);
        return t;
    }
}

namespace StringFunctions {
    /**
     * Finds the first parameter that begins with the string argPrefix
     * @param parameters all parameters, that should be searched
     * @param argPrefix beginning of the parameter that should be found
     * @return the parameter found without its prefix
     */
    string parseParameters(const std::vector<string> &parameters, const string &argPrefix) {
        auto checkArgument = [&argPrefix](const string &str) -> int {
            return str.compare(0, (str.length() < argPrefix.length()) ? string::npos : argPrefix.length(), argPrefix);
        };
        auto tmp = std::find_if_not(parameters.begin(), parameters.end(), checkArgument);
        return (tmp != parameters.end()) ? tmp->substr(argPrefix.length(), string::npos) : "";
    }
}