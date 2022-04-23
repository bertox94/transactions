#ifndef UTILS_UTILS_H
#define UTILS_UTILS_H

#include <string>
#include <unordered_map>
#include <sstream>
#include <chrono>

std::string trim(const std::string &str, const std::string &whitespace = " \t") {
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

//suppose the JSON is valid, not array and only string as values
std::unordered_map<string, string> JSONtomap(string json) {
    std::unordered_map<std::string, std::string> map = {};
    json = json.substr(1, json.length() - 2);

    for (int i = 0; i < json.length(); i++) {
        if (json[i] == '\"') {
            string key;
            i++;
            for (; json[i] != '\"'; i++)
                key += json[i];
            string value;
            i += 3;
            for (; json[i] != '\"'; i++)
                value += json[i];
            map[key] = value;
            i++;
        }
    }

    return map;
}

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

class testbench {
    chrono::time_point<chrono::steady_clock> _start;
    chrono::time_point<chrono::steady_clock> _end;
public:

    testbench() {
        _start = std::chrono::steady_clock::now();
    }

    void start() {
        _start = std::chrono::steady_clock::now();
    }

    long long int end() {
        _end = std::chrono::steady_clock::now();
        return (_end - _start).count();
    }
};

#endif //UTILS_UTILS_H