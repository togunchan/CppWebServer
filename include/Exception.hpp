#pragma once
#include <string>
#include <stdexcept>

/*
 * Base class for all server-side exceptions.
 * Inherits from std::runtime_error so it can carry a message.
 */
class ServerException : public std::runtime_error
{
public:
    explicit ServerException(const std::string &msg) : std::runtime_error(msg) {}
};

/*
 * Thrown when a socket operation fails.
 */
class SocketException : public ServerException
{
public:
    explicit SocketException(const std::string &msg) : ServerException("Socket Exception: " + msg) {}
};

/*
 * Thrown when HTTP parsing fails.
 */
class HttpParseException : public ServerException
{
public:
    explicit HttpParseException(const std::string &msg)
        : ServerException("HTTP Parse Error: " + msg) {}
};

/*
 * Thrown when a file cannot be found or read.
 */
class FileException : public ServerException
{
public:
    explicit FileException(const std::string &msg)
        : ServerException("File Error: " + msg) {}
};

class FileParseException : public ServerException
{
public:
    explicit FileParseException(const std::string &msg)
        : ServerException("File Parse Rrror: " + msg) {}
};