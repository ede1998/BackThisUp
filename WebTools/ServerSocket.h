//
// Created by Erik on 25.08.2017.
//


#pragma once
#include <vector>
#include <winsock2.h>

namespace WebTools {

    using buffer = std::vector<char>;

    class ServerSocket {
    public:
        ~ServerSocket();
        void bind(unsigned short port);
        void listen();
        unsigned short getPort();
        bool waitForConnections();
        buffer receiveData();
        bool send(const buffer &data);
    private:
        unsigned short m_port {0};
        bool m_connected {false};
        SOCKET m_Server {INVALID_SOCKET};
        SOCKET m_ClientCon {INVALID_SOCKET};
    };

    void initializeSockets();
    void deinitializeSockets();

    buffer stringToBuffer(const std::string &text);
    std::string bufferToString(const buffer &buf);
}