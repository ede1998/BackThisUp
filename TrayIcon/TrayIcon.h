//
// Created by Erik on 21.08.2017.
//


#pragma once

#include <string>
#include "resource.h"

namespace Backup {
    class TrayIcon {
    public:
        static TrayIcon * getInstance();
        void run();
        void changeDescription(const std::string &text);
    private:
        static HWND m_Hwnd;
        static HMENU m_Hmenu;
        static MSG m_messages;
        static NOTIFYICONDATA m_notifyIconData;
        static std::string m_text;
        static std::string m_className;

        static TrayIcon * m_instance;

        TrayIcon();
        HWND CreateDummyWindow(HINSTANCE hInstance, int iconId, LPCTSTR taskbarTitle);
        static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        void InitNotifyIconData();
    };
}