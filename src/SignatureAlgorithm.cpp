#include "SignatureAlgorithm.h"
#include "DataBufferStorage.h"
#include "InputModule.h"
#include "OutputModule.h"
#include "Utility/Logger.h"
#include "Utility/MD5.h"
#include "Utility/Measurement.h"
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include <ctime>

std::mutex threadCountMutex;

uint32_t maxThreadCount = 0;
uint32_t threadCount = 0;

static void increaseThreadCount()
{
    std::scoped_lock lg(threadCountMutex);
    ++threadCount;
}

static void decreaseThreadCount()
{
    std::scoped_lock lg(threadCountMutex);
    --threadCount;
}

static void updateMaxThreadCount()
{
    std::scoped_lock lg(threadCountMutex);
    maxThreadCount = std::max(threadCount, maxThreadCount);
}

SignatureAlgorithm::SignatureAlgorithm(const SignatureParams &params) :
    params(params),
    debugMode(params.debugMode),
    errorOccurred(false)
{}

static void finish(bool success)
{
    if (success) {
        Logger::writeLog("SUCCESS");
    } else {
        Logger::writeLog("Error occurred");
    }
}

void SignatureAlgorithm::start()
{
    InputModule inputModule;
    if (!inputModule.init(params.inputPath)) {
        errorOccurred = true;
        finish(false);
        return;
    }
    OutputModule outputModule;
    if (!outputModule.init(params.outputPath)) {
        errorOccurred = true;
        finish(false);
        return;
    }

    uint32_t initialBufferCount = params.threadCount;

    DataBufferStorage storage(params.blockSize, initialBufferCount);

    boost::asio::thread_pool pool(params.threadCount);

    uint64_t currentBlock = 0;
    uint64_t totalSize = 0;
    clock_t prevReadClock = clock();
    while (!inputModule.eof()) {
        if (errorOccurred) {
            break;
        }
        auto [dataBuffer, bufferId] = storage.getFreeBuffer();
        uint32_t readSize;
        auto prevStartReadTime = clock() - prevReadClock;
        prevReadClock = clock();
        auto readDuration = Measurement::measureFunc(
            [&readSize, &inputModule, dataBuf = dataBuffer, this] {
                readSize = inputModule.read(dataBuf->data(), params.blockSize);
                if (inputModule.eof()) {
                    std::fill(dataBuf->begin() + readSize, dataBuf->end(), 0);
                }
            });
        if (inputModule.isErrorOccurred()) {
            errorOccurred = true;
            break;
        }
        if (debugMode) {
            totalSize += readSize;
            Logger::writeLog(
                "Block: " + std::to_string(currentBlock) + ", read: " + std::to_string(readSize)
                + ", total: "
                + std::to_string(static_cast<double>(totalSize) / (1024.0 * 1024.0 * 1024.0)) + "Gb"
                + ", ReadTime: " + std::to_string(readDuration)
                + ", Time from previous read: " + std::to_string(prevStartReadTime)
                + ", eof: " + std::to_string(inputModule.eof()));
        }
        boost::asio::post(pool, [&storage, &outputModule, this, bufferId = bufferId, currentBlock] {
            if (errorOccurred) {
                return;
            }
            if (debugMode) {
                increaseThreadCount();
                updateMaxThreadCount();
            }
            std::string md5Result;
            auto funcDuration = Measurement::measureFunc(
                [&storage, &outputModule, &bufferId, &md5Result, &currentBlock] {
                    md5Result = md5(storage.getBuffer(bufferId)->data());
                    storage.releaseBuffer(bufferId);
                    outputModule.writeStr(currentBlock, md5Result);
                });
            if (outputModule.isErrorOccurred()) {
                errorOccurred = true;
            }

            if (debugMode) {
                Logger::writeLog("MD5: Block: " + std::to_string(bufferId) + ", result: "
                                 + md5Result + ", MD5Time: " + std::to_string(funcDuration));
                decreaseThreadCount();
            }
        });
        ++currentBlock;
    }

    pool.join();

    inputModule.close();
    outputModule.close();

    Logger::writeDebug("Max threads: " + std::to_string(maxThreadCount));
    Logger::writeDebug("Max output size: " + std::to_string(outputModule.getMaxMapSize()));
    Logger::writeDebug("All blocks count: " + std::to_string(currentBlock));

    finish(!errorOccurred);
}
