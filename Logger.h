//
// Created by Erik on 20.08.2017.
//


#pragma once


#include <iostream>
#include <deque>
#include <memory>

namespace Backup {
    enum ELevel {
        LVL_INFO, LVL_NORMAL, LVL_ERROR
    };

    class Logger {
    public:
        static Logger *getInstance();
        void log(std::string message, ELevel lvl);
    private:
        Logger() = default;

        static Logger *m_instance;

        struct Message {
            std::string msg;
            ELevel lvl;
        };
        std::deque<Message> m_messages;
    };
}