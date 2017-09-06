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

        auto mb = static_cast<WebTools::Curl::membuf *>(userp);

        int retcode = mb->sgetn(static_cast<char *>(ptr), size * nmemb);
#ifdef DEBUG
        nread = (curl_off_t) retcode;
        fprintf(stderr, "*** We read %" CURL_FORMAT_CURL_OFF_T
                " bytes from file\n", nread);
#endif
        return static_cast<size_t>(retcode);
    }

    size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userp) {
        auto data = static_cast<std::string *>(userp);
        auto cdata = new char[size * nmemb + 1];
        memcpy(cdata, ptr, size * nmemb);
        cdata[size * nmemb] = '\0';
        *data += cdata;
        return std::strlen(cdata);
    }
}

namespace WebTools {
    namespace Curl {
        CurlController::CurlController(const std::string &URL) {
            m_handle = curl_easy_init();
            m_init = m_handle != nullptr;
            if (!m_init)
                return;
            curl_easy_setopt(m_handle, CURLOPT_URL, URL.c_str());
            curl_easy_setopt(m_handle, CURLOPT_WRITEFUNCTION, write_callback);
            m_header.emplace_back("Expect:");
#ifdef DEBUG
            curl_easy_setopt(m_handle, CURLOPT_VERBOSE, 1L);
#endif
        }

        CurlController::~CurlController() {
            if (m_handle != nullptr)
                curl_easy_cleanup(m_handle);
        }

        bool CurlController::performUpload(membuf buf, const long long int fileSize) {
            curl_easy_setopt(m_handle, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(m_handle, CURLOPT_READFUNCTION, read_callback_file);
            curl_easy_setopt(m_handle, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(fileSize));
            curl_easy_setopt(m_handle, CURLOPT_READDATA, &buf);
            return perform();
        }

        bool CurlController::perform() {
            if (!m_init)
                return false;

            m_answer.clear();
            curl_easy_setopt(m_handle, CURLOPT_WRITEDATA, &m_answer);

            curl_slist *header = nullptr;
            for (const auto &hLine: m_header) {
                header = curl_slist_append(header, hLine.c_str());
            }
            curl_easy_setopt(m_handle, CURLOPT_HTTPHEADER, header);

            CURLcode res = curl_easy_perform(m_handle);

            curl_slist_free_all(header);
            return res == CURLE_OK;
        }

        bool CurlController::getInitStatus() {
            return m_init;
        }

        bool CurlController::performPOST(const std::string &data) {
            curl_easy_setopt(m_handle, CURLOPT_POST, 1L);
            curl_easy_setopt(m_handle, CURLOPT_POSTFIELDS, data.c_str());
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

        void CurlController::addHeader(const std::string &headerline) {
            size_t pos = headerline.find(':');
            removeHeader(headerline.substr(0, pos));
            m_header.push_back(headerline);
        }

        void CurlController::removeHeader(const std::string &headername) {
            for (auto el = std::begin(m_header); el != std::end(m_header); el++) {
                if (el->find(headername) == 0) {
                    m_header.erase(el);
                    break;
                }
            }
        }

        CurlController &CurlController::operator=(const CurlController &cc) {
            m_header = cc.m_header;
            m_answer = cc.m_answer;
            m_init = cc.m_init;
            m_handle = curl_easy_duphandle(cc.m_handle);
            return *this;
        }

        membuf::membuf(char *begin, char *end) {
            this->setg(begin, begin, end);
        }
    }
}