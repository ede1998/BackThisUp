//
// Created by Erik on 20.08.2017.
//


#pragma once


#include <iostream>
#include <deque>
#include <memory>
#include <fstream>
#include <set>
#include <ctime>
#include "../Functions.h"

namespace LoggingTools {
    enum ELevel {
        LVL_INFO, LVL_NORMAL, LVL_ERROR
    };

    class Logger {
    public:
        static Logger & getInstance();
        void logToConsole(bool shouldLog = true);
        void log(const std::string &message, ELevel lvl);
        bool save(const std::string &filePath);
        bool save(const std::string &filePath, const std::set<ELevel> &logLvl);
    private:
        Logger();
        ~Logger() = default;

        bool m_LogsToConsole;

        struct Message {
            std::string msg;
            ELevel lvl;
            std::time_t timestamp;
        };
        static std::string messageToString(const Message &msg);
        std::deque<Message> m_messages;
    };
}