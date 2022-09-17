#include "OutputModule.h"
#include "Logger.h"

std::mutex OutputModule::outputMutex;
OutputModule::OutputModule(bool debugMode) :
    debugMode(debugMode),
    expectedId(0)
{

}

bool OutputModule::init(const std::string &outputPath)
{
    std::scoped_lock lg(outputMutex);
    outputFile.open(outputPath, std::ios::out | std::ios::trunc);
    if (!outputFile.is_open()) {
        Logger::writeLog("Output file open error");
        return false;
    }
    return true;
}

void OutputModule::writeStr(uint64_t bufferId, const std::string &str)
{
    std::scoped_lock lg(outputMutex);

    if (bufferId == expectedId) {
        writeOutput(str);
        checkOutOfOrderStrings();
    } else {
        outOfOrderStrings[bufferId] = str;
    }
}

void OutputModule::close()
{
    std::scoped_lock lg(outputMutex);
    outputFile.close();
}

void OutputModule::writeOutput(const std::string &str)
{
    outputFile << str << "\n";
    ++expectedId;
}

void OutputModule::checkOutOfOrderStrings()
{
    while (!outOfOrderStrings.empty() && outOfOrderStrings.begin()->first == expectedId) {
        writeOutput(outOfOrderStrings.begin()->second);
        outOfOrderStrings.erase(outOfOrderStrings.begin());
    }
}
