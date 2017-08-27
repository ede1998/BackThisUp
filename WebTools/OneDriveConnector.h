//
// Created by Erik on 25.08.2017.
//


#pragma once
#include <string>
#include "ServerSocket.h"

#define DEFAULT_PORT 80
namespace WebTools {
    class OneDriveConnector {
    public:
        OneDriveConnector();
        bool upload(const std::string &localFile, const std::string &remoteFile);
        bool isCorrectlyInitialized();
    private:
        std::string m_token;
        std::string m_refreshToken;
        bool m_isInitialized;

        std::string requestCode();
        bool requestTokens(const std::string &code);
        bool refreshToken();
    };
}


