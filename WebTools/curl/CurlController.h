//
// Created by Erik on 31.08.2017.
//

#define DEBUG
#pragma once
#include "curllib/curl.h"
#include <string>
#include <vector>

class CurlController {
public:
    explicit CurlController(const std::string &URL);
    ~CurlController();
    bool performUpload(const std::string &filepath);
    bool performPOST(const std::vector<std::string> &headerOpt, const std::string &data);
    bool performDELETE();
    bool performGET();
    bool getInitStatus();
    std::string getLastAnswer();
private:
    bool m_init;
    CURL * m_handle;
    curl_slist * m_options;
    std::string m_answer;
    bool perform();
};



