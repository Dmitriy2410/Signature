#include "Logger.h"
#include "SignatureAlgorithm.h"
#include "SignatureParams.h"
#include <cstring>
#include <functional>
#include <memory>
#include <optional>

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

static const uint64_t MEGABYTE_MULTIPLIER = 1024 * 1024;

static uint64_t parseBlockSize(const std::string &argValue)
{
    static const uint64_t DEFAULT_BLOCK_SIZE = 1 * MEGABYTE_MULTIPLIER; // bytes

    auto unitItr = std::find_if(argValue.begin(), argValue.end(), [](auto s) {
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

enum class ArgType { None, Input, Output, BlockSize, DebugMode, ThreadCount };

ArgType parseArgType(char *arg)
{
    if (strcmp(arg, "-i") == 0) {
        return ArgType::Input;
    } else if (strcmp(arg, "-o") == 0) {
        return ArgType::Output;
    } else if (strcmp(arg, "-s") == 0) {
        return ArgType::BlockSize;
    } else if (strcmp(arg, "-d") == 0) {
        return ArgType::DebugMode;
    } else if (strcmp(arg, "-t") == 0) {
        return ArgType::ThreadCount;
    }
    return ArgType::None;
}

static bool parseArg(ArgType type, const std::string &argValue, SignatureParams &params)
{
    switch (type) {
    case ArgType::Input:
        params.inputPath = argValue;
        break;
    case ArgType::Output:
        params.outputPath = argValue;
        break;
    case ArgType::BlockSize:
        params.blockSize = parseBlockSize(argValue);
        break;
    case ArgType::ThreadCount:
        params.threadCount = std::stoi(argValue);
        break;
    case ArgType::DebugMode:
    default:
        Logger::writeLog("Unexpected state");
        return false;
    }

    return true;
}

static std::optional<SignatureParams> parseParams(int argc, char **argv)
{
    SignatureParams params;
    auto argType = ArgType::None;
    for (int i = 1; i < argc; ++i) {
        if (argType == ArgType::None) {
            argType = parseArgType(argv[i]);
            if (argType == ArgType::None) {
                Logger::writeLog("Unexpected argument: " + std::string(argv[i]));
                return {};
            }
            if (argType == ArgType::DebugMode) {
                params.debugMode = true;
                argType = ArgType::None;
            }
        } else {
            parseArg(argType, argv[i], params);
            argType = ArgType::None;
        }
    }
    return params;
}

static bool checkParam(bool expression, const std::string &errorMsg)
{
    if (expression) {
        Logger::writeLog(errorMsg);
        writeHelp();
        return false;
    }
    return true;
}

static const int MAX_THREAD_COUNT = 100;

int main(int argc, char **argv)
{
    auto paramsOpt = parseParams(argc, argv);
    if (!paramsOpt.has_value()) {
        return -1;
    }
    auto params = *paramsOpt;

    if (!checkParam(params.inputPath.empty(), "Invalid input file path")
        || !checkParam(params.outputPath.empty(), "Invalid output file path")
        || !checkParam(params.blockSize < 512 || params.blockSize > 10 * MEGABYTE_MULTIPLIER,
                       "Invalid block size")
        || !checkParam(params.threadCount < 1 || params.threadCount > MAX_THREAD_COUNT,
                       "Invalid thread count (expected 1 - " + std::to_string(MAX_THREAD_COUNT)
                           + ")")) {
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
