#include <iostream>
#include <forward_list>
#include <threads/mingw.thread.h>
#include "Functions.h"
#include "RootFolder.h"

using std::string;


void execute() {
    Backup::RootFolder rf("D:\\Users\\Erik\\CLionProjects\\BackThisUp", "D:\\Users\\Erik\\Desktop\\Comp");
    rf.backup("D:\\Users\\Erik\\Desktop\\Test");
}
int main() {
    using std::thread;
    thread tes(execute);
    tes.join();
    return 0;
}