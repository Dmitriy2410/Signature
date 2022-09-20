#include "DataBufferStorage.h"
#include "Logger.h"

std::mutex DataBufferStorage::bufferMutex;

DataBufferStorage::DataBufferStorage(uint64_t singleBufferSize, uint64_t initialBufferCount)
    : singleBufferSize(singleBufferSize)
    , bufferCount(initialBufferCount)
{
    buffer.reserve(bufferCount);
    fillNewBuffers(0, bufferCount);
}

std::pair<DataBufferStorage::SingleBufferPtr, uint64_t> DataBufferStorage::getFreeBuffer()
{
    std::scoped_lock lg(bufferMutex);
    if (freeBufferIds.empty()) {
        auto newBufferCount = 2 * bufferCount;
        buffer.reserve(newBufferCount);
        fillNewBuffers(bufferCount, newBufferCount);
        bufferCount = newBufferCount;
        Logger::writeLog("NewBufferCount: " + std::to_string(bufferCount));
    }
    auto freeBufferId = *freeBufferIds.begin();
    freeBufferIds.erase(freeBufferIds.begin());
    std::pair<SingleBufferPtr, uint64_t> freeBuffer = {buffer[freeBufferId], freeBufferId};
    return freeBuffer;
}

void DataBufferStorage::fillNewBuffers(size_t from, size_t to)
{
    for (uint64_t id = from; id < to; ++id) {
        buffer.push_back(std::make_shared<SingleBuffer>(singleBufferSize));
        freeBufferIds.insert(id);
    }
}

DataBufferStorage::SingleBufferPtr DataBufferStorage::getBuffer(uint64_t id) const
{
    std::scoped_lock lg(bufferMutex);
    auto bufferData = buffer[id];
    return bufferData;
}

void DataBufferStorage::releaseBuffer(uint64_t id)
{
    std::scoped_lock lg(bufferMutex);
    freeBufferIds.insert(id);
}

uint64_t DataBufferStorage::getIdDiff() const
{
    std::scoped_lock lg(bufferMutex);
    return bufferCount - freeBufferIds.size();
}
