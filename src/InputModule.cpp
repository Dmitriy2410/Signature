#include "InputModule.h"
#include "Utility/Logger.h"

InputModule::InputModule()
    : errorOccurred(false)
{}

bool InputModule::init(const std::string &filePath)
{
    Logger::writeDebug("InputModule::init");
    inputFile.open(filePath, std::ios::in | std::ios::binary);
    if (!inputFile.is_open()) {
        Logger::writeLog("Input file open error");
        return false;
    }
    if (inputFile.peek() == std::ifstream::traits_type::eof()) {
        Logger::writeLog("Input file is empty");
        return false;
    }

    return true;
}

bool InputModule::eof() const
{
    return inputFile.eof();
}

uint32_t InputModule::read(char *buffer, uint32_t blockSize)
{
    auto readSize = inputFile.read(buffer, blockSize).gcount();
    if (inputFile.fail()) {
        Logger::writeLog("InputModule: Input error occurred");
        errorOccurred = true;
    }
    return readSize;
}

void InputModule::close()
{
    Logger::writeDebug("InputModule::close");
    inputFile.close();
}

bool InputModule::isErrorOccurred() const
{
    return errorOccurred;
}
