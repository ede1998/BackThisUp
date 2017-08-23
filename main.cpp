#include <iostream>
#include <threads/mingw.thread.h>
#include <vector>
#include <algorithm>
#include "TrayIcon/TrayIcon.h"
#include "RootFolder.h"

int main(int argc, char **argv) {
    using namespace std;
    LoggingTools::Logger &loggerInstance = LoggingTools::Logger::getInstance();

    //parse input parameters
    vector<string> arguments(argv, argv + argc);
    arguments.erase(begin(arguments));
    string src    = StringFunctions::parseParameters(arguments, "-source=");
    string dest   = StringFunctions::parseParameters(arguments, "-destination=");
    string comp   = StringFunctions::parseParameters(arguments, "-compareDir=");
    string log    = StringFunctions::parseParameters(arguments, "-log=");
    string logLvl = StringFunctions::parseParameters(arguments, "-logLevel=");

    //silent mode
    if (find(begin(arguments), end(arguments), "-silent") != end(arguments)) {
        loggerInstance.log("Running in silent mode. No console output will be given.", LoggingTools::LVL_INFO);
        loggerInstance.logToConsole(false);
        FreeConsole();
    }

    //log parameters
    loggerInstance.log("Starting BackThisUp...", LoggingTools::LVL_INFO);
    loggerInstance.log("Source directory: " + src, LoggingTools::LVL_INFO);
    loggerInstance.log("Destination directory: " + dest, LoggingTools::LVL_INFO);
    loggerInstance.log("Comparison directory for differential backup: " + comp, LoggingTools::LVL_INFO);

    //initialize and start backup
    Backup::RootFolder rf;
    thread tBackup;
    time_t startTime = time(nullptr);
    try {
        rf = Backup::RootFolder(src, dest);
        tBackup = thread(rf.backup, &rf, comp);
    }
    catch (const invalid_argument &ia) {
        loggerInstance.log(ia.what(), LoggingTools::LVL_ERROR);
        return 0;
    }
    catch (const exception &e) {
        loggerInstance.log("unknown exception: " + string(e.what()), LoggingTools::LVL_ERROR);
    }

    //status information
    const unsigned int totalNumberFiles = rf.getFileCount();
    unsigned int currentNumberFiles = 0;
    unsigned int prevNumberFiles = 0;

    //initialize TrayIcon to display status information
    TrayIcon::TrayIcon ti("BackThisUp\nFortschritt: 0/" + to_string(totalNumberFiles));

    if (!ti.didFailToInitialize()) {
        //update status information until last file is processed
        while (currentNumberFiles != totalNumberFiles) {
            currentNumberFiles = rf.getFileProcessedCount();

            //only update if there was a change
            if (currentNumberFiles != prevNumberFiles) {
                ti.changeDescription("BackThisUp\nFortschritt: " + to_string(currentNumberFiles) + "/" + to_string(totalNumberFiles));
                prevNumberFiles = currentNumberFiles;
            }

            std::this_thread::yield();
        }
    }
    tBackup.join();
    TimeFunctions::time passedTime = TimeFunctions::secondsToTime(difftime(time(nullptr), startTime));

    loggerInstance.log("Total time: " + to_string(passedTime.hours) + " hour(s), " + to_string(passedTime.minutes) + " minute(s), " +
                       to_string(passedTime.seconds) + " second(s)", LoggingTools::LVL_INFO);
    //save log to file
    if (!log.empty()) {
        if (logLvl.empty()) {
            loggerInstance.save(log);
        } else {
            set<LoggingTools::ELevel> levels;

            transform(begin(logLvl), end(logLvl), begin(logLvl), ::tolower);

            if (logLvl.find("error") != string::npos)
                levels.insert(LoggingTools::LVL_ERROR);
            if (logLvl.find("info") != string::npos)
                levels.insert(LoggingTools::LVL_INFO);
            if (logLvl.find("error") != string::npos)
                levels.insert(LoggingTools::LVL_NORMAL);

            loggerInstance.save(log, levels);
        }
    }
    return 0;
}