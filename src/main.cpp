#include "SignatureParams.h"
#include "SignatureAlgorithm.h"
#include "Logger.h"
#include <functional>
#include <memory>

static void writeHelp()
{
    Logger::writeLog("");
    Logger::writeLog("Signature - file signature algorithm");
    Logger::writeLog("Args:");
    Logger::writeLog("-i - source file path");
    Logger::writeLog("-o - output file path");
    Logger::writeLog("-s - block size (512b - 10Mb)");
    Logger::writeLog("-t - thread count (Default: 15)");
    Logger::writeLog("-d - Enable debug mode");
}

static void parseArg(const std::string& arg, const std::string& argValue, const std::string& argName, const std::function<void(const std::string&)>& func) {
    if (arg == argName) {
        func(argValue);
    }
}

static const uint64_t MEGABYTE_MULTIPLIER = 1024 * 1024;

static uint64_t parseBlockSize(const std::string& argValue)
{
    static const uint64_t DEFAULT_BLOCK_SIZE = 1 * MEGABYTE_MULTIPLIER; // bytes

    auto unitItr = std::find_if(argValue.begin(), argValue.end(),
                                [] (auto s)
    {
        return !::isdigit(s);
    });
    auto unitPos = unitItr - argValue.begin();
    if (unitPos == 0) {
        return DEFAULT_BLOCK_SIZE;
    }

    auto value = std::stoull(argValue.substr(0, unitPos));
    auto unitStr = argValue.substr(unitPos, std::string::npos);

    if (unitStr == "b") {
        return value;
    } else if (unitStr == "kb") {
        return value * 1024;
    } else if (unitStr == "Mb") {
        return value * MEGABYTE_MULTIPLIER;
    }
    return 0;
}

static SignatureParams parseParams(int argc, char** argv)
{
    SignatureParams params;
    for (int i = 1; i < argc - 1; i = i+2) {
        auto arg = argv[i];
        auto argValue = argv[i+1];
        parseArg(arg, argValue, "-i", [&params] (auto& argValue) { params.inputPath = argValue; });
        parseArg(arg, argValue, "-o", [&params] (auto& argValue) { params.outputPath = argValue; });
        parseArg(arg, argValue, "-s", [&params] (auto& argValue) { params.blockSize = parseBlockSize(argValue); });
        parseArg(arg, argValue, "-d", [&params] (auto& argValue) { params.debugMode = argValue == "1"; });
    }
    return params;
}

static bool checkParam(bool expression, const std::string& errorMsg)
{
    if (expression) {
        Logger::writeLog(errorMsg);
        writeHelp();
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    auto params = parseParams(argc, argv);

    if (!checkParam(params.inputPath.empty(), "Invalid input file path") ||
            !checkParam(params.outputPath.empty(), "Invalid output file path") ||
            !checkParam(params.blockSize < 512 || params.blockSize > 10 * MEGABYTE_MULTIPLIER, "Invalid block size")) {
        return -1;
    }

    Logger::writeLog("Start algorithm");
    Logger::writeLog("input: " + params.inputPath);
    Logger::writeLog("output: " + params.outputPath);
    Logger::writeLog("block size: " + std::to_string(params.blockSize));

    auto alg = std::make_unique<SignatureAlgorithm>(params);
    alg->start();

    return 0;
}
