//
// Created by Erik on 31.08.2017.
//

#include <fstream>
#include <utility>
#include <cstring>
#include "CurlController.h"

namespace {
    size_t read_callback_file(void *ptr, size_t size, size_t nmemb, void *userp) {
        curl_off_t nread;

        auto fs = static_cast<std::ifstream *>(userp);
        if (!fs->is_open())
            return 0;
        fs->read(static_cast<char *>(ptr), size * nmemb);
        int i = fs->gcount();
        size_t retcode = static_cast<size_t>(i);
#ifdef DEBUG
        nread = (curl_off_t) retcode;
        if (nread != 0)
            if (!fs->good()) {
                fprintf(stderr, "***Error \n");
                if (fs->failbit)
                    fprintf(stderr, "failbit\n");
                if (fs->eofbit)
                    fprintf(stderr, "eof\n");
                if (fs->badbit)
                    fprintf(stderr, "badbit\n");
            }
        fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
                " bytes from file\n", nread);
#endif
        return retcode;
    }

    size_t write_callback(void * ptr, size_t size, size_t nmemb, void * userp) {
        auto data = static_cast<std::string *>(userp);
        auto cdata = new char[size * nmemb + 1];
        memcpy(cdata, ptr, size * nmemb);
        cdata[size * nmemb] = '\0';
        *data += cdata;
        return std::strlen(cdata);
    }

    std::ifstream::pos_type getFileSize(std::ifstream &f) {
        std::ifstream::pos_type size = f.seekg(0, std::ifstream::end).tellg();
        f.seekg(0, std::ifstream::beg);
        return size;
    }
}

CurlController::CurlController(const std::string &URL) {
    m_handle = curl_easy_init();
    m_options = nullptr;
    m_init = m_handle != nullptr;
    if (!m_init)
        return;
    curl_easy_setopt(m_handle, CURLOPT_URL, URL.c_str());
    curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, &m_answer);
    m_options = curl_slist_append(m_options, "Expect:");
#ifdef DEBUG
    curl_easy_setopt(m_handle, CURLOPT_VERBOSE, 1L);
#endif
}

CurlController::~CurlController() {
    if (m_handle != nullptr)
        curl_easy_cleanup(m_handle);
    curl_slist_free_all(m_options);
}

bool CurlController::performUpload(const std::string &filepath) {
    std::ifstream f(filepath, std::ifstream::binary);
    if (!f.is_open())
        return false;

    curl_easy_setopt(m_handle, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(m_handle, CURLOPT_READFUNCTION, read_callback_file);
    curl_easy_setopt(m_handle, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(getFileSize(f)));
    curl_easy_setopt(m_handle, CURLOPT_READDATA, &f);
    curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, m_options);

    bool res = perform();
    f.close();
    return res;
}

bool CurlController::perform() {
    if (!m_init)
        return false;
    m_answer = "";
    CURLcode res = curl_easy_perform(m_handle);
    if (res != CURLE_OK)
        return false;
    return true;
}

bool CurlController::getInitStatus() {
    return m_init;
}

bool CurlController::performPOST(const std::vector<std::string> &headerOpt, const std::string &data) {
    curl_easy_setopt(m_handle, CURLOPT_POST, 1L);
    curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, data.c_str());
    for (const auto &hLine: headerOpt) {
        m_options = curl_slist_append(m_options, hLine.c_str());
    }
    curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, m_options);
    return perform();
}

bool CurlController::performDELETE() {
    curl_easy_setopt(m_handle, CURLOPT_CUSTOMREQUEST, "DELETE ");
    return perform();
}

bool CurlController::performGET() {
    curl_easy_setopt(m_handle, CURLOPT_HTTPGET, 1L);

    return perform();
}

std::string CurlController::getLastAnswer() {
    return m_answer;
}
