#pragma once

#include <string>
#include <stdexcept>

struct Config
{
    int port;
    int sslPort;
    std::string docRoot;
    int maxThreads;

    /*
     * Static function to load configuration from a file.
     *
     * `static` allows calling `Config::load("filePath")` without
     * creating an instance of `Config`. It works independently
     * of any specific object.
     *
     * This method returns a new `Config` object, meaning it does
     * not rely on instance variables but rather constructs a new
     * configuration based on the given file.
     *
     */
    static Config load(const std::string &path);

    static std::string trim(const std::string &str);

    static std::string extractString(const std::string json, const std::string key);
    static int extractInt(const std::string json, const std::string key, int defaultValue = -1);
};
