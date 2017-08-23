//
// Created by Erik on 21.08.2017.
//

#include "TrayIcon.h"

namespace TrayIcon {
    int TrayIcon::m_uniqueID = 0;

    TrayIcon::TrayIcon(const std::string &txt)
            : m_initFail(true)
    {
        m_className = "UniqueString1234" + std::to_string(TrayIcon::m_uniqueID++);
        m_Hwnd = CreateDummyWindow(GetModuleHandle(nullptr), m_className.c_str());
        if (m_Hwnd == nullptr)
            return;

        InitNotifyIconData(txt);
        if (!Shell_NotifyIcon(NIM_ADD, &m_notifyIconData)) {
            CloseWindow(m_Hwnd);
            return;
        }
        m_initFail = false;
    }

    TrayIcon::~TrayIcon() {
        CloseWindow(m_Hwnd);
    }

    HWND TrayIcon::CreateDummyWindow(HINSTANCE hInstance, LPCTSTR taskBarTitle) const {
        WNDCLASSEX wincl{};        /* Data structure for the window class */
        /* The Window structure */
        wincl.hInstance = hInstance;
        wincl.lpszClassName = taskBarTitle;
        wincl.lpfnWndProc = DefWindowProc;      /* This function is called by windows */
        wincl.style = 0;
        wincl.cbSize = sizeof(WNDCLASSEX);

        /* Use default icon and mouse-pointer */
        wincl.hIcon = nullptr;
        wincl.hIconSm = nullptr;
        wincl.hCursor = nullptr;
        wincl.lpszMenuName = nullptr;                 /* No menu */
        wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
        wincl.cbWndExtra = 0;                      /* structure or the window instance */
        wincl.hbrBackground = nullptr;
        /* Register the window class, and if it fails quit the program */
        if (!RegisterClassEx(&wincl))
            return nullptr;

        /* The class is registered, let's create the program*/
        return CreateWindowEx(
                0,                   /* Extended possibilities for variation (ex style) */
                taskBarTitle,        /* Class name */
                nullptr,                /* Title Text */
                0,                   /* style flags */
                CW_USEDEFAULT,       /* Windows decides the position */
                CW_USEDEFAULT,       /* where the window ends up on the screen */
                0,                   /* The programs width */
                0,                   /* and height in pixels */
                nullptr,                /* parent handle */
                nullptr,                /* No menu */
                hInstance,           /* Program Instance handler */
                nullptr                 /* No Window Creation data */
        );
    }

    void TrayIcon::InitNotifyIconData(const std::string &txt) {
        memset(&m_notifyIconData, 0, sizeof(NOTIFYICONDATA));

        m_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
        m_notifyIconData.hWnd = m_Hwnd;
        m_notifyIconData.uID = ID_TRAY_APP_ICON;
        m_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        m_notifyIconData.uCallbackMessage = 0;
        m_notifyIconData.hIcon = (HICON) LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(ICO1));
        strncpy(m_notifyIconData.szTip, txt.c_str(), (unsigned int) std::max(64, (int) txt.length()));
    }


    void TrayIcon::changeDescription(const std::string &txt) {
        memset(&m_notifyIconData.szTip, 0, 64);
        strncpy(m_notifyIconData.szTip, txt.c_str(), (unsigned int) std::max(64, (int) txt.length()));
        Shell_NotifyIcon(NIM_MODIFY, &m_notifyIconData);
    }

    bool TrayIcon::didFailToInitialize() const {
        return m_initFail;
    }
}