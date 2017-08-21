#include <iostream>
#include <forward_list>
#include <threads/mingw.thread.h>
#include "Functions.h"
#include "RootFolder.h"

using std::string;

Backup::RootFolder * rf;

void execute() {
    rf->backup("D:\\Users\\Erik\\Desktop\\Test");
}

int main() {
    using std::thread;
    rf = new Backup::RootFolder("D:\\Users\\Erik\\CLionProjects\\BackThisUp", "D:\\Users\\Erik\\Desktop\\Comp");
    thread tes(execute);
    std::cout << "Number of files: " << rf->getFileCount() << std::endl;
    tes.join();
    delete rf;
    return 0;
}