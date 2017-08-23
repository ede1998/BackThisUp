#include <iostream>
#include <threads/mingw.thread.h>
#include "TrayIcon/TrayIcon.h"
#include "RootFolder.h"

using std::string;

bool done = false;

void execute(Backup::RootFolder * rf) {
    rf->backup(R"(D:\Users\Erik\Desktop\Test)");
    done = true;
}

int main(int argc, char** argv) {
    using std::thread;
    LoggingTools::Logger::getInstance().logToConsole(false);
    FreeConsole();
    Backup::RootFolder rf(R"(D:\Users\Erik\CLionProjects\BackThisUp)", R"(D:\Users\Erik\Desktop\Comp)");
    thread t(execute, &rf);
    unsigned int number = rf.getFileCount();
    TrayIcon::TrayIcon ti("BackThisUp\nFortschritt: 0/" + std::to_string(number));
    std::cout << "Number of files: " << number << std::endl;
    unsigned int processed = 0;
    while (!done){
        unsigned int tmp = rf.getFileProcessedCount();
        if (processed != tmp) {
            ti.changeDescription("BackThisUp\nFortschritt: " + std::to_string(tmp) + "/" + std::to_string(number));
            processed = tmp;
        }
        std::this_thread::yield();
    }
    t.join();
    LoggingTools::Logger::getInstance().save(R"(D:\Users\Erik\Desktop\log.txt)");
    LoggingTools::Logger::getInstance().save(R"(D:\Users\Erik\Desktop\logLvl.txt)", {LoggingTools::LVL_ERROR});
    return 0;
}