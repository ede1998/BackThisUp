//
// Created by Erik on 21.08.2017.
//


#pragma once

#include <string>
#include <windows.h>

namespace TrayIcon {
    class TrayIcon {
    public:
        explicit TrayIcon(const std::string &txt);
        ~TrayIcon();
        void changeDescription(const std::string &txt);
        bool didFailToInitialize() const;
    private:
        HWND m_Hwnd;
        NOTIFYICONDATA m_notifyIconData;
        std::string m_className;

        static int m_uniqueID;
        bool m_initFail;

        HWND CreateDummyWindow(HINSTANCE hInstance, LPCTSTR taskBarTitle) const;
        void InitNotifyIconData(const std::string &txt);
    };
}