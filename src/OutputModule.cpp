#include "OutputModule.h"
#include "Logger.h"

std::mutex OutputModule::outputMutex;
OutputModule::OutputModule() :
    expectedId(0),
    maxMapSize(0),
    errorOccured(false)
{

}

bool OutputModule::init(const std::string &outputPath)
{
    std::scoped_lock lg(outputMutex);
    outputFile.open(outputPath, std::ios::out | std::ios::trunc);
    if (!outputFile.is_open()) {
        Logger::writeLog("OutputModule: Output file open error");
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
        maxMapSize = std::max(maxMapSize, outOfOrderStrings.size());
    }
}

void OutputModule::close()
{
    std::scoped_lock lg(outputMutex);
    outputFile.close();
}

size_t OutputModule::getMaxMapSize() const
{
    std::scoped_lock lg(outputMutex);
    return maxMapSize;
}

bool OutputModule::isErrorOccured() const
{
    std::scoped_lock lg(outputMutex);
    return errorOccured;
}

void OutputModule::writeOutput(const std::string &str)
{
    outputFile << str << "\n";
    ++expectedId;
    if ((outputFile.rdstate() & std::fstream::failbit ) != 0) {
        Logger::writeLog("Output error occured");
        errorOccured = true;
    }
}

void OutputModule::checkOutOfOrderStrings()
{
    while (!outOfOrderStrings.empty() && outOfOrderStrings.begin()->first == expectedId) {
        writeOutput(outOfOrderStrings.begin()->second);
        outOfOrderStrings.erase(outOfOrderStrings.begin());
    }
}
