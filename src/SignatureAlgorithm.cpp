#include "SignatureAlgorithm.h"
#include "CommonUtils.h"
#include "DataBufferStorage.h"
#include "MD5.h"
#include <fstream>
#include <memory>
#include <unordered_map>
#include <thread>
#include <cassert>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio.hpp>

#include <ctime>

class Measurement {
public:
    static clock_t measureFunc(std::function<void()>&& func) {
        auto begTime = clock();
        if (func) {
            func();
        }
        return clock() - begTime;
    }
};

SignatureAlgorithm::SignatureAlgorithm(const SignatureParams &params) :
    params(params)
{

}

void SignatureAlgorithm::start()
{
    std::ifstream inputFile(params.inputPath, std::ios::in|std::ios::binary);
    if (!inputFile.is_open()) {
        writeLog("Input file open error");
        finish(false);
        return;
    }
    std::ofstream outputFile(params.outputPath, std::ios::out | std::ios::trunc);
    if (!outputFile.is_open()) {
        writeLog("Output file open error");
        finish(false);
        return;
    }

    uint64_t bufferCount = 1024.0 * 1024.0 * 1024.0 * 0.9 / params.blockSize;
    DataBufferStorage storage(params.blockSize, bufferCount);

    boost::asio::thread_pool pool(params.threadCount);

    uint64_t currentBlock = 0;
    uint64_t totalSize = 0;
    clock_t prevReadClock = clock();
    while (!inputFile.eof()) {
        auto [dataBuffer, bufferId] = storage.getFreeBuffer();
        uint64_t readSize;
        auto prevStartReadTime = clock() - prevReadClock;
        prevReadClock = clock();
        auto readDuration = Measurement::measureFunc([&readSize, &inputFile, &dataBuffer, this]
        {
            readSize = inputFile.read(dataBuffer->data(), params.blockSize).gcount();
            if (inputFile.eof()) {
                std::fill(dataBuffer->begin() + readSize, dataBuffer->end(), 0);
            }
        });
        totalSize += readSize;
        writeDebug("Block: " + std::to_string(currentBlock++) +
                   " read: " + std::to_string(readSize) +
                   " total: " + std::to_string(totalSize / (1024.0 * 1024.0 * 1024.0)) + "Gb" +
                   " time: " + std::to_string(readDuration) +
                   " Prev start read time: " + std::to_string(prevStartReadTime));

        boost::asio::post(pool, [&storage, &outputFile, this, bufferId] {
            std::string md5Result;
            auto funcDuration = Measurement::measureFunc([&storage, &outputFile, &bufferId, &md5Result, this]
            {
                md5Result = md5(storage.getBuffer(bufferId)->data());
                outputFile << md5Result << "\n";
            });

            writeDebug("MD5: Block: " + std::to_string(bufferId) + " result: " + md5Result + " time: " + std::to_string(funcDuration));
        });
    }

    inputFile.close();
    outputFile.close();

    pool.join();

    finish();
}

void SignatureAlgorithm::finish(bool success)
{
    if (success) {
        writeLog("SUCCESS");
    } else {
        writeLog("Error occured");
    }
}

std::mutex SignatureAlgorithm::debugMutex;
void SignatureAlgorithm::writeDebug(const std::string &msg)
{
    std::scoped_lock lg(debugMutex);
    if (params.debugMode) {
        writeLog(msg);
    }
}
