#include "SignatureAlgorithm.h"
#include "OutputModule.h"
#include "Logger.h"
#include "DataBufferStorage.h"
#include "MD5.h"
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

std::mutex diffMutex;
std::mutex threadCountMutex;

uint32_t maxThreadCount = 0;
uint32_t threadCount = 0;

static void increaseThreadCount() {
    std::scoped_lock lg(threadCountMutex);
    ++threadCount;
}
static void decreaseThreadCount() {
    std::scoped_lock lg(threadCountMutex);
    --threadCount;
}
static void updateMaxThreadCount() {
    std::scoped_lock lg(threadCountMutex);
    maxThreadCount = std::max(threadCount, maxThreadCount);
}


SignatureAlgorithm::SignatureAlgorithm(const SignatureParams &params) :
    params(params),
    debugMode(params.debugMode),
    errorOccurred(false)
{

}

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
    std::ifstream inputFile(params.inputPath, std::ios::in|std::ios::binary);
    if (!inputFile.is_open()) {
        Logger::writeLog("Input file open error");
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

    uint64_t initialBufferCount = params.threadCount;

    if (debugMode) {
        Logger::writeLog("DataBufferStorage: Init: " + std::to_string(initialBufferCount) + " buffers with " + std::to_string(params.blockSize) + " bytes");
    }
    DataBufferStorage storage(params.blockSize, initialBufferCount);

    boost::asio::thread_pool pool(params.threadCount);

    uint64_t currentBlock = 0;
    uint64_t totalSize = 0;
    clock_t prevReadClock = clock();
    std::map<uint64_t, uint64_t> differences;
    while (!inputFile.eof()) {
        if (errorOccurred) {
            break;
        }
        auto [dataBuffer, bufferId] = storage.getFreeBuffer();
        uint64_t readSize;
        auto prevStartReadTime = clock() - prevReadClock;
        prevReadClock = clock();
        auto readDuration = Measurement::measureFunc([&readSize, &inputFile, dataBuf = dataBuffer, this]
        {
            readSize = inputFile.read(dataBuf->data(), params.blockSize).gcount();
            if (inputFile.eof()) {
                std::fill(dataBuf->begin() + readSize, dataBuf->end(), 0);
            }
        });
        if (readSize == 0) {
            Logger::writeLog("Input error occurred");
            errorOccurred = true;
            break;
        }
        if (debugMode) {
            totalSize += readSize;
            Logger::writeLog("Block: " + std::to_string(currentBlock) +
                             " read: " + std::to_string(readSize) +
                             " total: " + std::to_string(static_cast<double>(totalSize) / (1024.0 * 1024.0 * 1024.0)) + "Gb" +
                             " time: " + std::to_string(readDuration) +
                             " Prev start read time: " + std::to_string(prevStartReadTime));
        }
        boost::asio::post(pool, [&storage, &outputModule, this, bufferId = bufferId, &differences, currentBlock] {
            if (errorOccurred) {
                return;
            }
            if (debugMode) {
                increaseThreadCount();
                updateMaxThreadCount();
            }
            std::string md5Result;
            auto funcDuration = Measurement::measureFunc([&storage, &outputModule, &bufferId, &md5Result, &currentBlock]
            {
                md5Result = md5(storage.getBuffer(bufferId)->data());
                storage.releaseBuffer(bufferId);
                outputModule.writeStr(currentBlock, md5Result);
            });
            if (outputModule.isErrorOccurred()) {
                errorOccurred = true;
            }

            if (debugMode) {
                Logger::writeLog("MD5: Block: " + std::to_string(bufferId) +
                                 " result: " + md5Result +
                                 " time: " + std::to_string(funcDuration));

                std::scoped_lock dm(diffMutex);
                differences[storage.getIdDiff()]++;
                decreaseThreadCount();
            }
        });
        ++currentBlock;
    }

    inputFile.close();
    outputModule.close();

    pool.join();

    if (debugMode) {
        std::for_each(differences.begin(), differences.end(),
                      [] (auto &itr)
        {
            Logger::writeLog(" Block difference: " + std::to_string(itr.first) + " - " + std::to_string(itr.second));
        });
        Logger::writeLog("Max threads: " + std::to_string(maxThreadCount));
        Logger::writeLog("Max output size: " + std::to_string(outputModule.getMaxMapSize()));
        Logger::writeLog("All blocks count: " + std::to_string(currentBlock));
    }

    finish(!errorOccurred);
}
