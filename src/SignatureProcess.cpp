#include "SignatureProcess.h"
#include "CommonUtils.h"
#include "MD5.h"
#include <fstream>

SignatureProcess::SignatureProcess(const SignatureParams &params, FinishFunc&& finishFunc) :
    params(params),
    finishFunc(std::move(finishFunc))
{

}

void SignatureProcess::start()
{
    std::ifstream inputFile(params.inputPath, std::ios::in|std::ios::binary);
    if (!inputFile.is_open()) {
        writeMsg("Input file open error");
        finish(false);
        return;
    }
    std::ofstream outputFile(params.outputPath, std::ios::out | std::ios::trunc);
    if (!outputFile.is_open()) {
        writeMsg("Output file open error");
        finish(false);
        return;
    }

    char memBlocks[params.blockSize];
    while (true) {
        auto readSize = inputFile.readsome(memBlocks, params.blockSize);
        if (inputFile.peek() == EOF) {
            for (uint64_t i = readSize; i < params.blockSize; ++i) {
                memBlocks[i] = 0;
            }
            outputFile << md5(memBlocks) << "\n";
            break;
        }
        outputFile << md5(memBlocks) << "\n";
    }
    inputFile.close();
    outputFile.close();

    finish();
}

void SignatureProcess::finish(bool success)
{
    if (success) {
        writeMsg("SUCCESS");
    } else {
        writeMsg("Error occured");
    }

    if (finishFunc) {
        finishFunc();
    }
}
