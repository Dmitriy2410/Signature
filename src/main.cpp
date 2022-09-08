#include "SignatureParams.h"
#include "CommonUtils.h"
#include <functional>

static void writeHelp()
{
    writeMsg("");
    writeMsg("Signature - file signature generator");
    writeMsg("Args:");
    writeMsg("-i - source file path");
    writeMsg("-o - output file path");
    writeMsg("-s - block size (512b - 10Mb)");
}

static void parseArg(const std::string& arg, const std::string& argValue, const std::string& argName, const std::function<void(const std::string&)>& func) {
    if (arg == argName) {
        func(argValue);
    }
}

static uint64_t parseBlockSize(const std::string& argValue)
{
    // todo: разбор единиц измерения
    return std::stoull(argValue);
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
    }
    return params;
}

int main(int argc, char** argv)
{
    auto params = parseParams(argc, argv);

    if (params.inputPath.empty() || params.outputPath.empty()) {
        writeHelp();
        return -1;
    }

    writeMsg("Start algorithm");
    writeMsg("input: " + params.inputPath);
    writeMsg("output: " + params.outputPath);
    writeMsg("size: " + std::to_string(params.blockSize));

    return 0;
}
