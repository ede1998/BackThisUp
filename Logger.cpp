//
// Created by Erik on 20.08.2017.
//

#include "Logger.h"

namespace Backup {

    Logger & Logger::getInstance() {
        static Logger m_instance;
        return m_instance;
    }

    void Logger::log(std::string message, ELevel lvl) {
        Message tmp;
        tmp.msg = message;
        tmp.lvl = lvl;
        m_messages.push_back(tmp);
        std::cout << message << std::endl;
    }
}
