#include <iostream>
#include <threads/mingw.thread.h>
#include <vector>
#include <algorithm>
#include <future>
#include "Definitions.h"
#include "TrayIcon/TrayIcon.h"
#include "Backup/RootFolder.h"
#include "WebTools/OneDriveConnector.h"

void displayHelp();
bool doBackup(const std::string &src, const std::string &dest, const std::string &comp, bool doIgnoreExcludes);
bool doOffsiteBack(const std::string &localPath, const std::string &remotePath, const std::string &pwd);

TrayIcon::TrayIcon trayIcon(PROGRAM_NAME);

int main(int argc, char **argv) {
    using namespace std;
    LoggingTools::Logger &loggerInstance = LoggingTools::Logger::getInstance();
    for (int i = 0; i < argc; i++) {
        loggerInstance.log(std::string(argv[i]), LoggingTools::LVL_INFO);
    }
    bool doIgnoreExcludes = false;

    //parse input parameters
    vector<string> arguments(argv, argv + argc);
    arguments.erase(begin(arguments));
    string src = StringFunctions::parseParameters(arguments, SOURCE_DIR);
    string dest = StringFunctions::parseParameters(arguments, DEST_DIR);
    string comp = StringFunctions::parseParameters(arguments, COMP_DIR);
    string log = StringFunctions::parseParameters(arguments, LOG_PATH);
    string logLvl = StringFunctions::parseParameters(arguments, LOG_LVL);
    string offsite = StringFunctions::parseParameters(arguments, OFFSITE_DIR);
    string pwd = StringFunctions::parseParameters(arguments, PWD);

    if (StringFunctions::doesParameterExist(arguments, HELP)) {
        displayHelp();
        return 0;
    }

    if (StringFunctions::doesParameterExist(arguments, SILENT)) {
        loggerInstance.log("Running in silent mode. No console output will be given.", LoggingTools::LVL_INFO);
        loggerInstance.logToConsole(false);
        FreeConsole();
    }

    if (StringFunctions::doesParameterExist(arguments, IGNORE_IGNORES)) {
        loggerInstance.log("Ignoring " EXCLUDE_NAME " files.", LoggingTools::LVL_NORMAL);
        doIgnoreExcludes = true;
    }

    //log parameters
    loggerInstance.log("Starting " PROGRAM_NAME "...", LoggingTools::LVL_INFO);
    loggerInstance.log("Source directory: " + src, LoggingTools::LVL_INFO);
    loggerInstance.log("Destination directory: " + dest, LoggingTools::LVL_INFO);
    loggerInstance.log("Comparison directory for differential backup: " + comp, LoggingTools::LVL_INFO);

    time_t startTime = time(nullptr);

    if (!FilesystemFunctions::createPath(dest))
        return false;

    if (!doBackup(src, dest, comp, doIgnoreExcludes))
        return 0;

    TimeFunctions::time passedTime = TimeFunctions::secondsToTime(difftime(time(nullptr), startTime));

    if (!offsite.empty()) {
        if (doOffsiteBack(dest, offsite, pwd))
            loggerInstance.log("Offsite backup successful.", LoggingTools::LVL_INFO);
        else
            loggerInstance.log("Offsite backup failed.", LoggingTools::LVL_ERROR);
    }

    loggerInstance.log("Total time: " + to_string(passedTime.hours) + " hour(s), " + to_string(passedTime.minutes) + " minute(s), " +
                       to_string(passedTime.seconds) + " second(s)", LoggingTools::LVL_INFO);

    //save log to file
    if (!log.empty()) {
        if (logLvl.empty()) {
            loggerInstance.save(log);
        } else {
            set<LoggingTools::ELevel> levels;

            transform(begin(logLvl), end(logLvl), begin(logLvl), ::tolower);

            if (logLvl.find(LOG_LVL_ERR) != string::npos)
                levels.insert(LoggingTools::LVL_ERROR);
            if (logLvl.find(LOG_LVL_INFO) != string::npos)
                levels.insert(LoggingTools::LVL_INFO);
            if (logLvl.find(LOG_LVL_NORM) != string::npos)
                levels.insert(LoggingTools::LVL_NORMAL);

            loggerInstance.save(log, levels);
        }
    }
    return 0;
}

void displayHelp() {
    using std::cout;
    using std::endl;
    cout << PROGRAM_NAME << endl;
    cout << SOURCE_DIR"SOURCE_PATH" << endl << "\tSpecifies from which folder data should be backed up." << endl;
    cout << DEST_DIR"DEST_PATH" << endl << "\tSpecifies the folder to backup to." << endl;
    cout << COMP_DIR"COMPARE_PATH" << endl << "\t Specifies a folder that contains a previous full backup for backing up differentially." << endl;
    cout << "\tLeave empty to do a full backup." << endl;
    cout << OFFSITE_DIR "OFFSITE_PATH" << endl << "\tIf given, the program will try to make an offsite backup to OneDrive." << endl;
    cout << "\tUse " PWD "PASSWORD to set a password for the uploaded backup archive." << endl;
    cout << HELP << endl << "\tYou know what this does." << endl;
    cout << SILENT << endl << "\tRun backup in silent mode. No console output will be given." << endl;
    cout << IGNORE_IGNORES << endl << "\tDon't use " EXCLUDE_NAME " files to select particular data that should not be backed up." << endl;
    cout << LOG_PATH"LOG_PATH" << endl << "\tSpecifies a file where the log should be written to. ";
    cout << "Leave empty to get no log file." << endl;
    cout << LOG_LVL LOG_LVL_ERR "," LOG_LVL_NORM "," LOG_LVL_INFO << endl << "\tAppend any log level to control what the log file should contain. ";
    cout << "Available log levels are: " LOG_LVL_ERR "," LOG_LVL_NORM "," LOG_LVL_INFO << endl;
    cout << "\tYou can use multiple log levels at once by separating them with a comma." << endl;
}

bool doBackup(const std::string &src, const std::string &dest, const std::string &comp, bool doIgnoreExcludes) {
    using namespace std;
    LoggingTools::Logger &loggerInstance = LoggingTools::Logger::getInstance();

    //initialize and start backup
    Backup::RootFolder rf;
    thread tBackup;
    try {
        rf = Backup::RootFolder(src, dest);
        rf.setIgnoreExcludes(doIgnoreExcludes);
        tBackup = thread(rf.backup, &rf, comp);
    }
    catch (const invalid_argument &ia) {
        loggerInstance.log(ia.what(), LoggingTools::LVL_ERROR);
        return false;
    }
    catch (const exception &e) {
        loggerInstance.log("unknown exception: " + string(e.what()), LoggingTools::LVL_ERROR);
    }

    //status information
    const unsigned int totalNumberFiles = rf.getFileCount();
    unsigned int currentNumberFiles = 0;
    unsigned int prevNumberFiles = 0;

    if (!trayIcon.didFailToInitialize()) {
        //update status information until last file is processed
        while (currentNumberFiles != totalNumberFiles) {
            currentNumberFiles = rf.getFileProcessedCount();

            //only update if there was a change
            if (currentNumberFiles != prevNumberFiles) {
                trayIcon.changeDescription(PROGRAM_NAME "\nFortschritt: " + to_string(currentNumberFiles) + "/" + to_string(totalNumberFiles));
                prevNumberFiles = currentNumberFiles;
            }

            std::this_thread::yield();
        }
    }
    tBackup.join();
    return true;
}

bool doOffsiteBack(const std::string &localPath, const std::string &remotePath, const std::string &pwd) {
    LoggingTools::Logger &loggerInstance = LoggingTools::Logger::getInstance();
    loggerInstance.log("Creating archive", LoggingTools::LVL_INFO);
    trayIcon.changeDescription(PROGRAM_NAME "\nCreating backup archive.");
    //Creating file to upload
    constexpr char createArchiveString[] = "\"~7zip~\" a ~name~.7z ~src~ -mhe";
    std::string command = StringFunctions::combineString(createArchiveString, {{"~7zip~", getApplicationPath() + ZIP_NAME},
                                                                               {"~name~", FilesystemFunctions::getTempFolder() + remotePath},
                                                                               {"~src~",  localPath}});
    if (!pwd.empty())
        command += " -p" + pwd;
    std::string res = exec(command.c_str());
    if (res.find("Everything is Ok") == std::string::npos) {
        loggerInstance.log("Could not create archive.", LoggingTools::LVL_ERROR);
        loggerInstance.log(command + " " + res, LoggingTools::LVL_NORMAL);
        return false;
    }

    loggerInstance.log("Successfully created archive.", LoggingTools::LVL_INFO);

    trayIcon.changeDescription(PROGRAM_NAME "\nUploading to OneDrive");


    WebTools::initializeSockets();
    //upload file
    WebTools::OneDriveConnector odc;
    WebTools::deinitializeSockets();
    if (!odc.isCorrectlyInitialized()) {
        loggerInstance.log("Could not connect to OneDrive.", LoggingTools::LVL_ERROR);
        return false;
    }
    bool successfullyUploaded;
    std::thread tUpload([&successfullyUploaded, &odc, &remotePath]() { successfullyUploaded = odc.upload(FilesystemFunctions::getTempFolder() + remotePath + ".7z", remotePath + ".7z"); });
    long long int lastProgress = -1;
    auto fsize = FilesystemFunctions::getFileSize(FilesystemFunctions::getTempFolder() + remotePath + ".7z");
    while (odc.getProgress() != fsize)
        if (odc.getProgress() != lastProgress) {
            lastProgress = odc.getProgress();
            trayIcon.changeDescription(PROGRAM_NAME "\nFortschritt: " + std::to_string(lastProgress / 1024) + "/" + std::to_string(fsize / 1024));
        }
    tUpload.join();
    if (successfullyUploaded)
        loggerInstance.log("Successfully uploaded backup.", LoggingTools::LVL_INFO);
    else
        loggerInstance.log("Could not upload backup.", LoggingTools::LVL_ERROR);
    return successfullyUploaded;
}
