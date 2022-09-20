#ifndef LOGGER_H
#define LOGGER_H

#include <atomic>
#include <mutex>
#include <string>

class Logger
{
public:
    static void writeLog(const std::string &msg);
    static void writeDebug(const std::string &msg);

    static void setupDebugMode();

private:
    static std::mutex logMutex;
    static std::atomic_bool debugMode;
};

#endif // LOGGER_H
