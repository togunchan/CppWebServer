#include "../include/ContentNegotiation.hpp"
#include "../include/Logger.hpp"
#include <sstream>

std::vector<std::string> splitAndTrim(const std::string s)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, ','))
    {
        auto start = token.find_first_not_of(" \r\n\t");
        auto end = token.find_last_not_of(" \r\n\t");
        if (start == std::string::npos || end == std::string::npos)
            continue;
        token = token.substr(start, end - start + 1);
        tokens.push_back(token);
    }
    return tokens;
}

bool isAcceptable(const std::string &acceptHeader, const std::string &mimeType)
{
    if (acceptHeader.empty())
        return true;

    auto tokens = splitAndTrim(acceptHeader);
    for (const auto &token : tokens)
    {
        auto semicolon = token.find(';');
        log("Header is " + token);
        std::string mediaRange = (semicolon == std::string::npos) ? token : token.substr(0, semicolon);

        // If mediaRange == "*/*", allow everything
        if (mediaRange == "*/*")
            return true;

        // If mediaRange ends with "/*" (e.g., "text/*") and mimeType starts with the same prefix, allow it
        auto slashPos = mediaRange.find('/');
        if (slashPos != std::string::npos && mediaRange.back() == '*')
        {
            std::string typePrefix = mediaRange.substr(0, slashPos);
            if (mimeType.rfind(typePrefix + '/', 0) == 0)
                return true;
        }

        if (mediaRange == mimeType)
            return true;
    }

    return false;
}