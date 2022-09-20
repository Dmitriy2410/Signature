#include "Logger.h"
#include <iostream>

std::mutex Logger::logMutex;

void Logger::writeLog(const std::string &msg)
{
    std::scoped_lock lg(logMutex);
    std::cout << msg << std::endl;
}
