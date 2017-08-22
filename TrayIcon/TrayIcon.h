//
// Created by Erik on 21.08.2017.
//


#pragma once

#include <string>
#include <threads/mingw.thread.h>
#include <atomic>
#include "resource.h"

namespace Backup {
    class TrayIcon {
    public:
        explicit TrayIcon(const std::string &txt);
        ~TrayIcon();
        void changeDescription(const std::string &txt);
    private:
        HWND m_Hwnd;
        NOTIFYICONDATA m_notifyIconData;
        std::string m_className;

        static int m_uniqueID;

        HWND CreateDummyWindow(HINSTANCE hInstance, LPCTSTR taskBarTitle);
        void InitNotifyIconData(const std::string &txt);
    };
}