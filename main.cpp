#include <iostream>
#include <forward_list>
#include <threads/mingw.thread.h>
#include "TrayIcon/TrayIcon.h"
#include "Functions.h"
#include "RootFolder.h"

using std::string;

void execute(Backup::RootFolder &rf) {
    std::this_thread::sleep_for(std::chrono::seconds(500));
    rf.backup(R"(D:\Users\Erik\Desktop\Test)");
}

int main() {
    using std::thread;
    Backup::RootFolder rf(R"(D:\Users\Erik\CLionProjects\BackThisUp)", R"(D:\Users\Erik\Desktop\Comp)");
    thread t(execute, rf);
    unsigned int number = rf.getFileCount();
    Backup::TrayIcon ti("BackThisUp");
    ti.changeDescription("Fortschritt " + std::to_string(number));
    std::cout << "Number of files: " << number << std::endl;
    t.join();
    return 0;
}