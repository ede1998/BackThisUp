//
// Created by Erik on 25.08.2017.
//

#include <stdexcept>
#include <ws2tcpip.h>
#include "ServerSocket.h"

namespace {
    void copy(WebTools::buffer::const_iterator  beg, WebTools::buffer::const_iterator end, char * target, int len) {
        int i = 0;
        for (auto iter = beg; iter < end; iter++) {
            if (len-- <= 0) return;
            target[i] = *iter;
            i++;
        }
    }
}

namespace WebTools {
    ServerSocket::~ServerSocket() {
        if (m_ClientCon != INVALID_SOCKET) {
            shutdown(m_ClientCon, SD_BOTH); //SOCKET_ERROR
            closesocket(m_ClientCon);
        }
        if (m_Server != INVALID_SOCKET)
            closesocket(m_Server);
    }

    void ServerSocket::bind(unsigned short port) {
        if (m_Server != INVALID_SOCKET)
            return;
        if (m_connected)
            return;

        m_port = port;

        addrinfo *addr = nullptr;
        addrinfo hints{0};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        if (getaddrinfo(nullptr, std::to_string(port).c_str(), &hints, &addr) != 0)
            throw std::runtime_error("Failed to bind socket.");

        m_Server = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);


        if (m_Server == INVALID_SOCKET)
            throw std::runtime_error("Failed to bind socket.");

        if (::bind(m_Server, addr->ai_addr, (int) addr->ai_addrlen) == SOCKET_ERROR) {
            freeaddrinfo(addr);
            throw std::runtime_error("Failed to bind socket.");
        }
        freeaddrinfo(addr);
    }

    void ServerSocket::listen() {
        if (::listen(m_Server, 1) == SOCKET_ERROR) {
            closesocket(m_Server);
            m_Server = INVALID_SOCKET;
            throw std::runtime_error("Failed to listen.");
        }
    }

    unsigned short ServerSocket::getPort() {
        return m_port;
    }

    bool ServerSocket::waitForConnections() {
        if (m_ClientCon != INVALID_SOCKET)
            return false;
        if (m_Server == INVALID_SOCKET)
            return false;
        // Accept a client socket
        m_ClientCon = accept(m_Server, nullptr, nullptr);

        m_connected = m_ClientCon != INVALID_SOCKET;
        return m_connected;
    }

    buffer ServerSocket::receiveData() {
        constexpr int recvbuflen = 512;
        char recvbuf[recvbuflen];
        int iResult;
        buffer res;
        unsigned long hasData;
        do {
            if (ioctlsocket(m_ClientCon, FIONREAD, &hasData) != 0)
                return res;
            if (hasData > 0)
                iResult = recv(m_ClientCon, recvbuf, recvbuflen, 0);
            else
                iResult = 0;
            if (iResult > 0)
                //received iResult bytes of data
                for (int i = 0; i < iResult; i++)
                    res.push_back(recvbuf[i]);
        } while (iResult > 0);
        return res;
    }

    bool ServerSocket::send(const buffer &data) {
        constexpr int recvbuflen = 128;
        char recvbuf[recvbuflen];
        int iSendResult, totalSent = 0;
        int length{std::distance(std::begin(data), std::end(data))};

        // Receive until the peer shuts down the connection
        while (totalSent < length) {
            auto endCopy = std::min(std::begin(data) + totalSent + recvbuflen, std::end(data));
            copy(std::begin(data) + totalSent, endCopy, recvbuf, recvbuflen);
            iSendResult = ::send(m_ClientCon, recvbuf, std::min(recvbuflen, length - totalSent), 0);
            totalSent += iSendResult;
            if (iSendResult == SOCKET_ERROR)
                return false;
        }
        return true;
    }

    void initializeSockets() {
        WSAData wsadata{};
        if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
            throw std::runtime_error("Could not initialize WinSock.");
    }

    void deinitializeSockets() {
        WSACleanup();
    }

    buffer stringToBuffer(const std::string &text) {
        buffer res;
        res.resize(text.length());
        std::copy(std::begin(text), std::end(text), std::begin(res));
        return res;
    }

    std::string bufferToString(const buffer &buf) {
        return std::string(std::begin(buf), std::end(buf));
    }
}