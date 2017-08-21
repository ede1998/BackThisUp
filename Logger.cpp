//
// Created by Erik on 20.08.2017.
//

#include "Logger.h"

namespace Backup {

    Logger * Logger::m_instance;

    Logger * Logger::getInstance() {
        if (m_instance == nullptr)
            m_instance = new Logger();
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
