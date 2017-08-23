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

    void Logger::log(std::string message, ELevel lvl) {
        Message tmp;
        tmp.msg = message;
        tmp.lvl = lvl;
        tmp.timestamp = time(0);
        m_messages.push_back(tmp);
        if (m_LogsToConsole)
            std::cout << TimeFunctions::timeToString(tmp.timestamp)<< "\t" << lvl << "\t" << message << std::endl;
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
                fs << TimeFunctions::timeToString(m.timestamp) << "\t" << m.lvl << "\t" << m.msg << std::endl;
        }
        fs.close();
        return fs.failbit == std::ios::goodbit;
    }

    void Logger::logToConsole(bool shouldLog) {
        m_LogsToConsole = shouldLog;
    }
}
