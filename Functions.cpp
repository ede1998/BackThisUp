
#include <memory>
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
    string findAbsolutePath(const string &absRoot, const string &relPath) { //TODO: use absolute path
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

    std::ifstream::pos_type getFileSize(std::ifstream &f) {
        std::ifstream::pos_type size = f.seekg(0, std::ifstream::end).tellg();
        f.seekg(0, std::ifstream::beg);
        return size;
    }

    std::ifstream::pos_type getFileSize(const std::string &path) {
        std::ifstream f(path);
        if (!f.is_open())
            return 0;
        auto size = getFileSize(f);
        f.close();
        return size;
    }

    std::string loadFileToString(const std::string &filePath) {
        std::string res;
        std::ifstream inp(filePath);
        if (inp.is_open()) {
            res.assign((std::istreambuf_iterator<char>(inp)), (std::istreambuf_iterator<char>()));
            inp.close();
        }
        return res;
    }

    bool saveStringToFile(const std::string &filePath, const std::string &content) {
        std::ofstream out(filePath);
        if (out.is_open()) {
            out << content;
            out.close();
        } else
            return false;
        return true;
    }

    void processFileByChunk(const std::string &path, const unsigned int chunkSize, std::function<void (char *, const unsigned int)> processingFu) {
        std::ifstream bigFile(path);
        auto buffer = new char[chunkSize];
        try {
            while (bigFile) {
                bigFile.read(buffer, chunkSize);
                int tmp = bigFile.gcount();
                processingFu(buffer, std::min(chunkSize, static_cast<const unsigned int>(tmp)));
            }
        } catch (...) {
            delete buffer;
            throw;
        }
        delete buffer;
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

    bool doesParameterExist(const std::vector<string> &parameters, const string &argName) {
        return std::find(std::begin(parameters), std::end(parameters), argName) != std::end(parameters);
    }

    std::string combineString(std::string inputStr, const std::map<std::string, std::string> &args) {
        size_t pos;
        for (const auto &arg: args) {
            pos = 0;
            while (pos < inputStr.length()) {
                pos = inputStr.find(arg.first, pos);
                if (pos >= inputStr.length())
                    break;
                inputStr.replace(pos, arg.first.length(), arg.second);
                pos += arg.first.length();
            }
        }
        return inputStr;
    }
}

std::string exec(const char *cmd) {
    std::array<char, 128> buffer{};
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (feof(pipe.get()) == 0) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
            result += buffer.data();
    }
    return result;
}