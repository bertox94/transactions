#include <string>
#include <unordered_map>
#include <sstream>

using namespace std;

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

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}