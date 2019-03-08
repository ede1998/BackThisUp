//
// Created by Erik on 14.12.2017.
//

#include "Manager.h"

namespace Backup {
    Manager::Manager(const std::string &absPathSrc, const std::string &absPathDest)
    : m_rf(absPathSrc, absPathDest),
      m_totalThreads(10),
      m_threads(m_totalThreads)
    {
    }

    void Manager::setTotalTreads(unsigned int nr) {
        m_totalThreads = nr;
        for (int i = m_threads.size(); m_threads.size() < nr; i++)
            m_threads.emplace_back(std::thread());
    }

    void Manager::init() {
        m_rf.init();
    }

    void Manager::backup(const std::string &compPath) {

    }
}
