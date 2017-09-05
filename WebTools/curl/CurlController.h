//
// Created by Erik on 31.08.2017.
//

#define DEBUG
#pragma once
#include "curllib/curl.h"
#include <string>
#include <vector>

namespace Curl {
    class membuf;

    class CurlController {
    public:
        explicit CurlController(const std::string &URL);
        CurlController(const CurlController &cc) = delete;
        CurlController & operator=(const CurlController &cc);
        ~CurlController();
        bool performUpload(membuf buf, long long int fileSize);
        bool performPOST(const std::string &data = "");
        bool performDELETE();
        bool performGET();
        bool getInitStatus();
        void addHeader(const std::string &headerline);
        void removeHeader(const std::string &headername);
        std::string getLastAnswer();
    private:
        bool m_init;
        CURL *m_handle;
        std::vector<std::string> m_header;
        std::string m_answer;
        bool perform();
    };


    class membuf : public std::streambuf {
    public:
        membuf(char *begin, char *end);
    };

}
