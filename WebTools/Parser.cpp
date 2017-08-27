//
// Created by Erik on 25.08.2017.
//

#include <iterator>
#include <sstream>
#include <fstream>
#include <set>
#include <algorithm>

#include "Parser.h"

namespace {
    bool shorten(std::string &text, const char &begTrim, const char &endTrim) {
        if (!((text.front() == begTrim) && (text.back() == endTrim)))
            return false;
        text = text.substr(1, text.length() - 2);
        return true;
    }

    std::string trim(const std::string &text) {
        std::size_t beg = text.find_first_not_of(" \t\r\n");
        std::size_t end = text.find_last_not_of(" \t\r\n");
        if (beg > text.length())
            return "";
        return text.substr(beg, end - beg +1);
    }
}

namespace WebTools {
    JSONObject::JSONObject(const std::string &text) :
            m_invalidObject{false} {
        parseJSON(text);
    }

    std::string JSONObject::getValue(const std::string &key) {
        return m_elements.find(key)->second;
    }

    bool JSONObject::hasKey(const std::string &key) {
        return (m_elements.find(key) != std::end(m_elements))
               || (m_recursiveElements.find(key) != std::end(m_recursiveElements));
    }

    JSONObject JSONObject::getValueJSON(const std::string &key) {
        return m_recursiveElements.find(key)->second;
    }

    std::vector<std::string> split(const std::string &text, const char &delimiter) {
        int openBraces = 0;
        int openBrackets = 0;
        bool quotesOpened = false;
        std::vector<std::string> commaSplitted;
        std::stringstream tmp;
        for (const char &c: text) {
            switch (c) {
                case '{':
                    openBraces++;
                    break;
                case '}':
                    openBraces--;
                    break;
                case '[':
                    if (openBraces == 0)
                        openBrackets++;
                    break;
                case ']':
                    if (openBraces == 0)
                        openBrackets--;
                    break;
                case '"':
                    if ((openBraces == 0) && (openBrackets == 0))
                        quotesOpened = !quotesOpened;
                    break;
            }

            if ((openBraces == 0) && (openBrackets == 0) && !quotesOpened && (c == delimiter)) {
                commaSplitted.push_back(tmp.str());
                tmp.str("");
            } else
                tmp << c;
        }
        commaSplitted.push_back(tmp.str());
        return commaSplitted;
    }

    void JSONObject::parseJSON(std::string text) {

        shorten(text, '{', '}');
        text = trim(text);

        std::vector<std::string> data = split(text, ',');

        for (const std::string &completePair: data) {
            std::vector<std::string> splittedPair = split(completePair, ':');

            //1 key has exactly one value
            if (std::distance(std::begin(splittedPair), std::end(splittedPair)) != 2) {
                m_invalidObject = true;
                return;
            }

            //remove excess characters
            for (std::string &part: splittedPair) {
                part = trim(part);
                shorten(part, '"', '"');
            }

            //add to found elements
            if (shorten(splittedPair[1], '{', '}'))
                m_recursiveElements.insert(std::pair<std::string, JSONObject>(splittedPair[0], JSONObject(splittedPair[1])));
            else
                m_elements.insert(std::pair<std::string, std::string>(splittedPair[0], splittedPair[1]));
        }

    }

    bool JSONObject::isValid() {
        return m_invalidObject;
    }
}