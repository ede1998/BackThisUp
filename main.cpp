#include <iostream>
#include <forward_list>
#include <threads/mingw.thread.h>
#include "TrayIcon/TrayIcon.h"
#include "Functions.h"
#include "RootFolder.h"

using std::string;


void execute(Backup::RootFolder &rf) {
    std::this_thread::sleep_for(std::chrono::seconds(20));
    rf.backup(R"(D:\Users\Erik\Desktop\Test)");
}

void execute1(Backup::TrayIcon * * ti) {
    (*ti) = new Backup::TrayIcon();
    (*ti)->run();
    delete *ti;
}

int main() {
    using std::thread;
    Backup::RootFolder rf(R"(D:\Users\Erik\CLionProjects\BackThisUp)", R"(D:\Users\Erik\Desktop\Comp)");
    thread t(execute, rf);
    Backup::TrayIcon * ti = nullptr;
    thread t1(execute1, &ti);
    t1.detach();
    while (ti == nullptr);
    ti->changeDescription("neue beschriftung");
    unsigned int number = rf.getFileCount();
    std::cout << "Number of files: " << number << std::endl;
    t.join();
    return 0;
}