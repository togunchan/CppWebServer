#include "../include/Logger.hpp"
#include <mutex>
#include <iostream>

std::mutex logMutex;

void log(const std::string &message)
{
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << message
              << std::endl;
}