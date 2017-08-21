#include <iostream>
#include <forward_list>
#include <threads/mingw.thread.h>
#include "TrayIcon/TrayIcon.h"
#include "Functions.h"
#include "RootFolder.h"

using std::string;

bool initdone = false;

void execute(Backup::RootFolder &rf) {
    std::this_thread::sleep_for(std::chrono::seconds(20));
    rf.backup(R"(D:\Users\Erik\Desktop\Test)");
}

void execute1() {
    Backup::TrayIcon * ti = Backup::TrayIcon::getInstance();
    initdone = true;
    ti->run();
}

int main() {
    using std::thread;
    Backup::RootFolder rf(R"(D:\Users\Erik\CLionProjects\BackThisUp)", R"(D:\Users\Erik\Desktop\Comp)");
    thread t(execute, rf);
    thread t1(execute1);
    t1.detach();
    while (!initdone);
    Backup::TrayIcon::getInstance()->changeDescription("neue beschriftung");
    unsigned int number = rf.getFileCount();
    std::cout << "Number of files: " << number << std::endl;
    t.join();
    return 0;
}