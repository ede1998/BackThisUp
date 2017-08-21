#include <iostream>
#include <forward_list>
#include "Functions.h"
#include "RootFolder.h"

using std::string;
/*int main() {
    std::cout << "Hello, World!" << std::endl;
    string path_absolute = "D:\\Users\\Erik\\CLionProjects\\googletest\\";
    string path_relative = "..\\BackThisUp\\cmake-build-debug\\..";
    string res = Backup::findAbsolutePath(path_absolute, path_relative);
    std::cout << res << std::endl;
    std::forward_list<string> files = Backup::findFiles(res);
    std::forward_list<string> folders = Backup::findFolders(res);
    for (string el: files)
        std::cout << el << std::endl;
    for (string el: folders)
        std::cout << el << std::endl;
    Backup::RootFolder rf("D:\\Users\\Erik", "");
    return 0;
}*/

int main() {
    Backup::RootFolder rf("D:\\Users\\Erik\\CLionProjects\\BackThisUp", "D:\\Users\\Erik\\Desktop\\Comp");
    rf.backup("D:\\Users\\Erik\\Desktop\\Test");
    return 0;
}