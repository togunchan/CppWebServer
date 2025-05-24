#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include "../include/Logger.hpp"

std::string Config::trim(const std::string &str)
{
    std::string whiteSpace = " \t\n\r\f\v";
    size_t start = str.find_first_not_of(whiteSpace);
    size_t end = str.find_last_not_of(whiteSpace);
    if (start == std::string::npos || end == std::string::npos)
        return "";
    return str.substr(start, end - start + 1);
}

std::string Config::extractString(const std::string json, const std::string key)
{
    /*
     *  Example of JSON format
     *  {
     *   "port": 8080,
     *   "docRoot": "./public/",
     *   "maxThreads": 4
     *  }
     */
    size_t pos = json.find('"' + key + '"');
    if (pos == std::string::npos)
        throw std::runtime_error("Key not found in JSON: " + key);

    pos = json.find(':', pos);
    if (pos == std::string::npos)
        throw std::runtime_error("Colon not found in JSON: " + key);

    size_t start = json.find('"', pos + 1);
    size_t end = json.find('"', start + 1);
    if (start == std::string::npos || end == std::string::npos)
    {
        throw std::runtime_error("Malformed string value for key: " + key);
    }
    auto value = json.substr(start + 1, end - start - 1);
    return trim(value);
}

int Config::extractInt(const std::string json, const std::string key, int defaultValue)
{
    /*
     *  Example of JSON format
     *  {
     *   "port": 8080,
     *   "docRoot": "./public/",
     *   "maxThreads": 4
     *  }
     */
    size_t pos = json.find('"' + key + '"');
    if (pos == std::string::npos)
        return defaultValue;

    pos = json.find(':', pos);
    if (pos == std::string::npos)
        throw std::runtime_error("Malformed JSON after key: " + key);

    pos++; // move past ':'

    // extract substring until non-digit
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\t'))
        pos++;

    auto end = pos;
    while (end < json.size() && std::isdigit(json[end]) || json[end] == '-')
        end++;
    int val = std::stoi(json.substr(pos, end - pos));

    return val;
}

Config Config::load(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Failed to open config-file: " + path);

    std::ostringstream buf;
    buf << file.rdbuf();
    std::string json = buf.str();

    Config config;
    config.port = extractInt(json, "port", 8080);
    if (config.port <= 0 || config.port > 65535)
        throw std::runtime_error("Invalid port number: " + std::to_string(config.port));
    config.docRoot = extractString(json, "docRoot");
    if (config.docRoot.empty())
        throw std::runtime_error("docRoot is empty");
    config.maxThreads = extractInt(json, "maxThreads", 4);

    return config;
}
