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
#include "sha1/sha1.hpp"
#include "curl/CurlController.h"

namespace {
    void OpenWebsite(const std::string &cpURL) {
        ShellExecute(nullptr, "open", cpURL.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    }

    constexpr char RequestCodeString[] = "https://login.microsoftonline.com/common/oauth2/v2.0/authorize?client_id=~client_id~&scope=~scopes~&response_type=code&redirect_uri=~redirect_uri~";
    constexpr char RequestTokenString[] = R"(~curl~ "https://login.microsoftonline.com/common/oauth2/v2.0/token" -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "client_id=~client_id~&redirect_uri=~redirect_uri~&client_secret=~client_secret~&code=~code~&grant_type=authorization_code" -k)";
    constexpr char RefreshTokenString[] = R"(~curl~ "https://login.microsoftonline.com/common/oauth2/v2.0/token" -X POST -H "Content-Type: application/x-www-form-urlencoded" -d "client_id=~client_id~&redirect_uri=~redirect_uri~&client_secret=~client_secret~&refresh_token=~refresh_token~&grant_type=refresh_token" -k)";
    constexpr char CreateUploadString[] = R"(~curl~ "https://graph.microsoft.com/v1.0/me/drive/root:/~remote_path~:/createUploadSession" -X POST -H "Authorization: Bearer ~access_token~" -H "Content-Length: 0" -k)";
    constexpr char UploadFileString[] = R"(~curl~ "~url~" -X PUT -H "Content-Length: ~block_length~" -H "Content-Range: bytes ~begin_range~-~end_range~/~total_length~" -T "~path~" -k -v)";
    constexpr char UploadCancelString[] = R"(~curl~ "~url~" -X DELETE -k)";
    constexpr char UploadFileStringParts[] = R"(~curl~ "~url~" -X PUT -H "Content-Length: ~block_length~" -H "Content-Range: bytes ~begin_range~-~end_range~/~total_length~" -v -k --data-binary ~data~)";
    constexpr char UploadSmallFileString[] = R"(~curl~ "https://graph.microsoft.com/v1.0/me/drive/root:/~remote_path~:/content" -X PUT -H "Content-Type: plain/text" -H "Authorization: Bearer ~access_token~" -T "~path~" -k)";

    constexpr char URLToken[] = R"(https://login.microsoftonline.com/common/oauth2/v2.0/token)";
    constexpr char URLUpload[] = R"(https://graph.microsoft.com/v1.0/me/drive/root:/~remote_path~:/createUploadSession)";
    constexpr char TokenRefreshData[] = R"(client_id=~client_id~&redirect_uri=~redirect_uri~&client_secret=~client_secret~&refresh_token=~refresh_token~&grant_type=refresh_token)";
    constexpr char TokenRequestData[] = R"(client_id=~client_id~&redirect_uri=~redirect_uri~&client_secret=~client_secret~&code=~code~&grant_type=authorization_code)";
    constexpr char Token[] = R"(Authorization: Bearer ~access_token~)";
    constexpr char ContentRange[] = R"(Content-Range: bytes ~begin_range~-~end_range~/~total_length~)";
    constexpr char ContentLength[] = R"(Content-Length: ~length~)";

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

            Curl::CurlController cc(StringFunctions::combineString(URLUpload, {{"~remote_path~", remoteFile}}));
            if (!cc.getInitStatus())
                return false;
            cc.addHeader(StringFunctions::combineString(Token, {{"~access_token~", m_token}}));
            if (!cc.performPOST()) {
                return false;
            }
            answer = cc.getLastAnswer();

        JSONObject parsedAnswer(answer);
        const std::string uploadURL = parsedAnswer.getValue("uploadUrl");

        if (uploadURL.empty())
            return false;

        const long long int fileSize = FilesystemFunctions::getFileSize(localFile);
        if (fileSize == 0)
            return false;

        std::string sha1 = SHA1::from_file(localFile);
        std::transform(std::begin(sha1), std::end(sha1), std::begin(sha1), toupper);


        int totalSent = 0;
        cc = Curl::CurlController(uploadURL);
        if (!cc.getInitStatus())
            return false;


        std::function<void(char *, const unsigned int)> lambda = [&](char *buf, const unsigned int chunkSize) -> void {
            cc.addHeader(StringFunctions::combineString(ContentRange, {{"~begin_range~",  std::to_string(totalSent)},
                                                                       {"~end_range~",    std::to_string((totalSent + chunkSize - 1))},
                                                                       {"~total_length~", std::to_string(fileSize)}}));
            cc.addHeader(StringFunctions::combineString(ContentLength, {{"~length~", std::to_string(chunkSize)}}));

            totalSent += chunkSize;
            if (!cc.performUpload(Curl::membuf(buf, buf + chunkSize), fileSize))
                return;
            //answer = cc.getLastAnswer(); //TODO: maybe process answer
        };

        FilesystemFunctions::processFileByChunk(localFile, 1024 * 1024 * 32, lambda);
        answer = cc.getLastAnswer();

        parsedAnswer = JSONObject(answer);


        bool successful = parsedAnswer.getValue("name") == remoteFile;
        successful = successful && parsedAnswer.getValue("size") == std::to_string(fileSize);
        successful = successful && parsedAnswer.getValueJSON("file").getValueJSON("hashes").getValue("sha1Hash") == sha1;


        if (!successful) {
            cc = Curl::CurlController(uploadURL);
            cc.performDELETE();
        }

        return successful;
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
        Curl::CurlController cc(URLToken);
        if (!cc.getInitStatus())
            return false;
        if (!cc.performPOST(StringFunctions::combineString(TokenRequestData, {{"~redirect_uri~",  RedirectURI},
                                                                              {"~client_id~",     client_id},
                                                                              {"~client_secret~", client_secret}}))) {
            return false;
        }
        std::string answer = cc.getLastAnswer();
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
        Curl::CurlController cc(URLToken);
        if (!cc.getInitStatus())
            return false;
        if (!cc.performPOST(StringFunctions::combineString(TokenRefreshData, {{"~redirect_uri~",  RedirectURI},
                                                                              {"~client_id~",     client_id},
                                                                              {"~client_secret~", client_secret},
                                                                              {"~refresh_token~", m_refreshToken}}))) {
            return false;
        }
        std::string answer = cc.getLastAnswer();
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