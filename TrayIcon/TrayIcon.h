//
// Created by Erik on 21.08.2017.
//


#pragma once

#include <string>
#include "resource.h"

namespace Backup {
    class TrayIcon {
    public:
        TrayIcon();

        void run();

        void changeDescription(const std::string &text);

    private:
        HWND m_Hwnd;
        HMENU m_Hmenu;
        MSG m_messages;
        NOTIFYICONDATA m_notifyIconData;
        std::string m_text;
        std::string m_className;

        HWND CreateDummyWindow(HINSTANCE hInstance, int iconId, LPCTSTR taskbarTitle);
        void InitNotifyIconData();
    };
}