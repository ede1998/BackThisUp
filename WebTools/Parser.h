//
// Created by Erik on 25.08.2017.
//


#pragma once
#include <Map>
#include <string>
#include <vector>

namespace WebTools {
    class JSONObject {
    public:
        explicit JSONObject(const std::string &text);
        JSONObject();
        bool isValid();

        bool hasKey(const std::string &key);
        std::string getValue(const std::string &key);
        JSONObject getValueJSON(const std::string &key);
    private:
        std::map<std::string, std::string> m_elements;
        std::map<std::string, JSONObject> m_recursiveElements;
        bool m_invalidObject;

        void parseJSON(std::string text);
    };

}