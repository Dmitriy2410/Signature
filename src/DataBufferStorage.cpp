#include "DataBufferStorage.h"
#include "Logger.h"

std::mutex DataBufferStorage::bufferMutex;

DataBufferStorage::DataBufferStorage(uint64_t singleBufferSize, uint64_t maxBufferCount) :
    singleBufferSize(singleBufferSize),
    maxBufferCount(maxBufferCount),
    freeBufferId(0)
{
    Logger::writeLog("DataBufferStorage: Init: " + std::to_string(maxBufferCount) + " buffers with " + std::to_string(singleBufferSize) + " bytes");

    buffer.reserve(maxBufferCount);
    for (uint64_t id = 0; id < maxBufferCount; ++id) {
        buffer.push_back(std::make_shared<SingleBuffer>(singleBufferSize));
    }
}

std::pair<DataBufferStorage::SingleBufferPtr, uint64_t> DataBufferStorage::getFreeBuffer()
{
    std::scoped_lock lg(bufferMutex);
    std::pair<SingleBufferPtr,uint64_t> freeBuffer = {buffer[freeBufferId], freeBufferId};
    rotateFreeBufferId();
    return freeBuffer;
}

DataBufferStorage::SingleBufferPtr DataBufferStorage::getBuffer(uint64_t id) const
{
    std::scoped_lock lg(bufferMutex);
    auto bufferData = buffer[id];
    return bufferData;
}

uint64_t DataBufferStorage::getIdDiff(uint64_t id) const
{
    std::scoped_lock lg(bufferMutex);
    if (freeBufferId < id) {
        return maxBufferCount - id + freeBufferId;
    } else {
        return freeBufferId - id;
    }
}

void DataBufferStorage::rotateFreeBufferId()
{
    ++freeBufferId;
    if (freeBufferId == maxBufferCount) {
        freeBufferId = 0;
    }
}
