#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <string>

class Logger
{
public:
    static void writeLog(const std::string &msg);

private:
    static std::mutex logMutex;
};

#endif // LOGGER_H
