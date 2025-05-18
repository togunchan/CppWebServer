#ifndef LOGGER_HP
#define LOGGER_HP

#include <string>

/*
 * Thread-safe logging function.
 * Uses a mutex to ensure only one thread at a time writes to std::cout,
 * preventing interleaved or garbled output.
 */
void log(const std::string &message);

#endif // LOGGER_HP