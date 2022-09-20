#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <string>
#include <atomic>

class Logger
{
public:
    static void writeLog(const std::string &msg);
    static void writeDebug(const std::string &msg);

private:
    static std::mutex logMutex;
    static std::atomic_bool debugMode;
};

#endif // LOGGER_H
