//
// Created by Erik on 20.08.2017.
//

#include "Logger.h"

namespace LoggingTools {

    Logger::Logger()
            : m_LogsToConsole(true) {
    }

    Logger &Logger::getInstance() {
        static Logger m_instance;
        return m_instance;
    }

    void Logger::log(const std::string &message, ELevel lvl) {
        Message tmp;
        tmp.msg = message;
        tmp.lvl = lvl;
        tmp.timestamp = time(nullptr);
        m_messages.push_back(tmp);
        if (m_LogsToConsole)
            std::cout << messageToString(tmp) << std::endl;
    }

    bool Logger::save(const std::string &filePath) {
        return save(filePath, {LVL_INFO, LVL_ERROR, LVL_NORMAL});
    }

    bool Logger::save(const std::string &filePath, const std::set<ELevel> &logLvl) {
        std::ofstream fs(filePath, std::ios::app);
        if (!fs.is_open())
            return false;
        for (Message m: m_messages) {
            if (logLvl.find(m.lvl) != std::end(logLvl))
                fs << messageToString(m) << std::endl;
        }
        fs.close();
        return fs.failbit == std::ios::goodbit;
    }

    void Logger::logToConsole(bool shouldLog) {
        m_LogsToConsole = shouldLog;
    }

    std::string Logger::messageToString(const Logger::Message &msg) {
        std::string lvl;
        switch (msg.lvl) {
            case LVL_ERROR:
                lvl = "Error";
                break;
            case LVL_NORMAL:
                lvl = "Default";
                break;
            case LVL_INFO:
                lvl = "Information";
                break;
            default:
                lvl = "Unknown";
                break;
        }
        return TimeFunctions::timeToString(msg.timestamp) + "\t" + lvl + "\t" + msg.msg;
    }
}
