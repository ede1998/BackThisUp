//
// Created by Erik on 21.08.2017.
//

#include "TrayIcon.h"

namespace Backup {

    std::string TrayIcon::m_className;
    HWND TrayIcon::m_Hwnd;
    HMENU TrayIcon::m_Hmenu;
    MSG TrayIcon::m_messages;
    NOTIFYICONDATA TrayIcon::m_notifyIconData;
    std::string TrayIcon::m_text;
    TrayIcon * TrayIcon::m_instance;

    TrayIcon::TrayIcon() {
        m_className = "UniqueString1234";
        m_Hwnd = CreateDummyWindow(GetModuleHandle(NULL), ICO1, m_className.c_str());

        InitNotifyIconData();
        Shell_NotifyIcon(NIM_ADD, &m_notifyIconData);
    }

    HWND TrayIcon::CreateDummyWindow(HINSTANCE hInstance, int iconId, LPCTSTR taskbarTitle) {
        WNDCLASSEX wincl{};        /* Data structure for the windowclass */
        /* The Window structure */
        wincl.hInstance = hInstance;
        wincl.lpszClassName = taskbarTitle;
        wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
        wincl.style = 0;
        wincl.cbSize = sizeof(WNDCLASSEX);

        /* Use default icon and mouse-pointer */
        wincl.hIcon = NULL;
        wincl.hIconSm = NULL;
        wincl.hCursor = NULL;
        wincl.lpszMenuName = NULL;                 /* No menu */
        wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
        wincl.cbWndExtra = 0;                      /* structure or the window instance */
        wincl.hbrBackground = NULL;
        /* Register the window class, and if it fails quit the program */
        if (!RegisterClassEx(&wincl))
            return NULL;

        /* The class is registered, let's create the program*/
        return CreateWindowEx(
                0,                   /* Extended possibilites for variation (ex style) */
                taskbarTitle,        /* Classname */
                NULL,                /* Title Text */
                0,                   /* style flags */
                CW_USEDEFAULT,       /* Windows decides the position */
                CW_USEDEFAULT,       /* where the window ends up on the screen */
                0,                   /* The programs width */
                0,                   /* and height in pixels */
                NULL,                /* parent handle */
                NULL,                /* No menu */
                hInstance,           /* Program Instance handler */
                NULL                 /* No Window Creation data */
        );
    }

    void TrayIcon::InitNotifyIconData() {
        memset(&m_notifyIconData, 0, sizeof(NOTIFYICONDATA));

        m_notifyIconData.cbSize = sizeof(NOTIFYICONDATA);
        m_notifyIconData.hWnd = m_Hwnd;
        m_notifyIconData.uID = ID_TRAY_APP_ICON;
        m_notifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        m_notifyIconData.uCallbackMessage = WM_SYSICON; //Set up our invented Windows Message
        m_notifyIconData.hIcon = (HICON) LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(ICO1));
        strncpy(m_notifyIconData.szTip, m_text.c_str(), (unsigned int) std::max(64, (int) m_text.length()));
    }

    void TrayIcon::run() {
        while (GetMessage(&m_messages, NULL, 0, 0)) {
            /* Translate virtual-key messages into character messages */
            TranslateMessage(&m_messages);
            /* Send message to WindowProcedure */
            DispatchMessage(&m_messages);
        }
    }

    void TrayIcon::changeDescription(const std::string &text) {
        m_text = text;
        ModifyMenu(m_Hmenu, ID_TRAY_INFO, MF_STRING, ID_TRAY_INFO, m_text.c_str());
        memset(&m_notifyIconData.szTip, 0, 64);
        strncpy(m_notifyIconData.szTip, m_text.c_str(), (unsigned int) std::max(64, (int) m_text.length()));
    }

    LRESULT CALLBACK TrayIcon::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message)                  /* handle the messages */
        {
            case WM_CREATE:
                m_Hmenu = CreatePopupMenu();
                AppendMenu(m_Hmenu, MF_GRAYED, ID_TRAY_INFO, TEXT("blulb"));
                AppendMenu(m_Hmenu, MF_STRING, ID_TRAY_EXIT, TEXT("Abort backup"));
                break;
            case WM_SYSICON:    // self defined WM_SYSICON message.
            {
                if ((lParam == WM_RBUTTONDOWN) || (lParam == WM_LBUTTONUP)) {
                    // Get current mouse position.
                    POINT curPoint;
                    GetCursorPos(&curPoint);

                    // TrackPopupMenu blocks the app until TrackPopupMenu returns
                    UINT clicked = TrackPopupMenu(m_Hmenu, TPM_RETURNCMD | TPM_NONOTIFY, curPoint.x, curPoint.y, 0,
                                                  hwnd,
                                                  NULL);

                    SendMessage(hwnd, WM_NULL, 0, 0); // send benign message to window to make sure the menu goes away.
                    if (clicked == ID_TRAY_EXIT) {
                        // quit the application.
                        Shell_NotifyIcon(NIM_DELETE, &m_notifyIconData);
                        PostQuitMessage(0);
                    }
                }
            }
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                break;
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    TrayIcon * TrayIcon::getInstance() {
        if (m_instance == nullptr)
            m_instance = new TrayIcon();
        return m_instance;
    }
}