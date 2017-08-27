//
// Created by Erik on 25.08.2017.
//

#include "OneDriveConnector.h"

#include <memory>
#include <fstream>
#include <map>
#include "pw.h"
#include "Parser.h"
#include <sstream>
#include <iterator>
#include <algorithm>
#include "../Functions.h"
#include "../Definitions.h"

namespace {
    void OpenWebsite(const std::string &cpURL) {
        ShellExecute(nullptr, "open", cpURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }

    constexpr char RequestCodeString[] = "https://login.microsoftonline.com/common/oauth2/v2.0/authorize?client_id=~client_id~&scope=~scopes~&response_type=code&redirect_uri=~redirect_uri~";
    constexpr char RequestTokenString[] = R"(~curl~ "https://login.microsoftonline.com/common/oauth2/v2.0/token" -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "client_id=~client_id~&redirect_uri=~redirect_uri~&client_secret=~client_secret~&code=~code~&grant_type=authorization_code" -k)";
    constexpr char RefreshTokenString[] = R"(~curl~ "https://login.microsoftonline.com/common/oauth2/v2.0/token" -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "client_id=~client_id~&redirect_uri=~redirect_uri~&client_secret=~client_secret~&refresh_token=~refresh_token~&grant_type=refresh_token" -k)";
    constexpr char CreateUploadString[] = R"(~curl~ "https://graph.microsoft.com/v1.0/me/drive/root:/~remote_path~:/createUploadSession" -X POST -H "Authorization: Bearer ~access_token~" -H "Content-Length: 0" -k)";
    constexpr char UploadFileString[] = R"(~curl~ "~url~" -X PUT -H "Content-Length: ~total_length~" -H "Content-Range: bytes ~begin_range~-~end_range~/~total_length~" -T "~path~" -k)";
    constexpr char UploadSmallFileString[] = R"(~curl~ "https://graph.microsoft.com/v1.0/me/drive/root:/~remote_path~:/content" -X PUT -H "Content-Type: plain/text" -H "Authorization: Bearer ~access_token~" -T "~path~" -k)";

    constexpr char RedirectURI[] = "http://localhost";
    constexpr char DefaultHeader[] = "HTTP/1.1 200 OK\r\nExpires: -1\r\nContent-Type: text/html; charset=ISO-8859-1\r\n\r\n";
}

namespace WebTools {
    OneDriveConnector::OneDriveConnector()
            : m_refreshToken(FilesystemFunctions::loadFileToString(REFRESH_TOKEN_PATH)),
              m_token(""),
              m_isInitialized(false) {
        if (!m_refreshToken.empty())
            m_isInitialized = refreshToken();
        else {
            std::string code = requestCode();
            if (!code.empty())
                if (requestTokens(code)) {
                    m_isInitialized = true;
                }
        }
        FilesystemFunctions::saveStringToFile(REFRESH_TOKEN_PATH, m_refreshToken);
    }

    bool OneDriveConnector::upload(const std::string &localFile, const std::string &remoteFile) {
        if (!m_isInitialized)
            return false;
        std::string answer;
//#define SMALL_FILES
#ifdef SMALL_FILES
        answer = exec(StringFunctions::combineString(UploadSmallFileString, {{"~curl~",         CURL_PATH},
                                                      {"~access_token~", m_token},
                                                      {"~remote_path~",  remoteFile},
                                                      {"~path~",         localFile}}).c_str());
#else
        answer = exec(StringFunctions::combineString(CreateUploadString, {{"~curl~",         CURL_PATH},
                                                                          {"~remote_path~",  remoteFile},
                                                                          {"~access_token~", m_token}}).c_str());
        JSONObject parsedAnswer(answer);
        const std::string uploadURL = parsedAnswer.getValue("uploadUrl");

        if (uploadURL.empty())
            return false;

        const long long int fileSize = FilesystemFunctions::getFileSize(localFile);
        if (fileSize == 0)
            return false;

        answer = exec(StringFunctions::combineString(UploadFileString, {{"~curl~",         CURL_PATH},
                                                                        {"~url~",          uploadURL},
                                                                        {"~total_length~", std::to_string(fileSize)},
                                                                        {"~begin_range~",  "0"},
                                                                        {"~end_range~",    std::to_string(fileSize - 1)},
                                                                        {"~path~",         localFile}}).c_str());
        parsedAnswer = JSONObject(answer);

        //TODO: check return for sh1 hash, compare, check size, filename,...?
        bool successful = parsedAnswer.getValue("name") == remoteFile;
        successful = successful && parsedAnswer.getValue("size") == std::to_string(fileSize);
        return successful;
#endif
        return true;
    }

    std::string OneDriveConnector::requestCode() {
        const WebTools::buffer answer = WebTools::stringToBuffer(std::string(DefaultHeader) + FilesystemFunctions::loadFileToString(ANSWER_PAGE_PATH));
        const std::string requestString = StringFunctions::combineString(RequestCodeString, {{"~redirect_uri~", RedirectURI},
                                                                                             {"~client_id~",    client_id},
                                                                                             {"~scopes~",       "files.readwrite+offline_access"}});
        WebTools::buffer recvData;

        {
            //initialize socket to receive code
            WebTools::ServerSocket ss;
            ss.bind(DEFAULT_PORT);
            ss.listen();

            //get code and serve page
            OpenWebsite(requestString);
            ss.waitForConnections();
            while (recvData.empty())
                recvData = ss.receiveData();
            ss.send(answer);
        }//end of scope frees socket -> data is sent directly (issue with ServerSocket)

        //split received data by spaces to extract code
        std::stringstream tmp(WebTools::bufferToString(recvData));
        std::vector<std::string> header((std::istream_iterator<std::string>(tmp)),
                                        std::istream_iterator<std::string>());

        //check if HTTP header with code in URL
        if (header[0] != "GET")
            return "";
        if (header[1].find("/?code=") != 0)
            return "";

        return header[1].substr(7, std::string::npos);

    }

    bool OneDriveConnector::requestTokens(const std::string &code) {
        //make token request and parse request result
        std::string answer = exec(StringFunctions::combineString(RequestTokenString, {{"~curl~",          CURL_PATH},
                                                                                      {"~redirect_uri~",  RedirectURI},
                                                                                      {"~client_id~",     client_id},
                                                                                      {"~client_secret~", client_secret},
                                                                                      {"~code~",          code}}).c_str());
        JSONObject parsedAnswer(answer);

        //extract access token
        std::string value = parsedAnswer.getValue("access_token");
        if (!value.empty())
            m_token = value;
        else
            return false;

        //extract refresh token
        value = parsedAnswer.getValue("refresh_token");
        if (!value.empty())
            m_refreshToken = value;
        else
            return false;

        return true;
    }

    bool OneDriveConnector::refreshToken() {
        //request new token and parse result
        std::string answer = exec(StringFunctions::combineString(RefreshTokenString, {{"~curl~",          CURL_PATH},
                                                                                      {"~redirect_uri~",  RedirectURI},
                                                                                      {"~client_id~",     client_id},
                                                                                      {"~client_secret~", client_secret},
                                                                                      {"~refresh_token~", m_refreshToken}}).c_str());
        JSONObject parsedAnswer(answer);

        //extract refresh token
        std::string value = parsedAnswer.getValue("refresh_token");
        if (!value.empty())
            m_refreshToken = value;

        //extract access token
        value = parsedAnswer.getValue("access_token");
        if (!value.empty())
            m_token = value;

        return !value.empty();
    }

    bool OneDriveConnector::isCorrectlyInitialized() {
        return m_isInitialized;
    }
}