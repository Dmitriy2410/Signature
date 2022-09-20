#include "Logger.h"
#include <iostream>

std::atomic_bool Logger::debugMode = false;

std::mutex Logger::logMutex;
void Logger::writeLog(const std::string &msg)
{
    std::scoped_lock lg(logMutex);
    std::cout << msg << std::endl;
}

void Logger::writeDebug(const std::string &msg)
{
    if (debugMode) {
        writeLog(msg);
    }
}

void Logger::setupDebugMode()
{
    debugMode = true;
}
