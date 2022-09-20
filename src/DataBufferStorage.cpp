#include "DataBufferStorage.h"
#include "Utility/Logger.h"

std::mutex DataBufferStorage::bufferMutex;

DataBufferStorage::DataBufferStorage(uint32_t singleBufferSize, uint32_t initialBufferCount)
    : singleBufferSize(singleBufferSize)
    , bufferCount(initialBufferCount)
{
    Logger::writeDebug("DataBufferStorage::Init " + std::to_string(initialBufferCount)
                       + " buffers with " + std::to_string(singleBufferSize) + " bytes");
    buffer.reserve(bufferCount);
    fillNewBuffers(0, bufferCount);
}

std::pair<DataBufferStorage::SingleBufferPtr, uint32_t> DataBufferStorage::getFreeBuffer()
{
    std::scoped_lock lg(bufferMutex);
    if (freeBufferIds.empty()) {
        auto newBufferCount = 2 * bufferCount;
        buffer.reserve(newBufferCount);
        fillNewBuffers(bufferCount, newBufferCount);
        bufferCount = newBufferCount;
        Logger::writeDebug("NewBufferCount: " + std::to_string(bufferCount));
    }
    auto freeBufferId = *freeBufferIds.begin();
    freeBufferIds.erase(freeBufferIds.begin());
    return {buffer[freeBufferId], freeBufferId};
}

void DataBufferStorage::fillNewBuffers(size_t from, size_t to)
{
    for (auto id = from; id < to; ++id) {
        buffer.push_back(std::make_shared<SingleBuffer>(singleBufferSize));
        freeBufferIds.insert(id);
    }
}

DataBufferStorage::SingleBufferPtr DataBufferStorage::getBuffer(uint32_t id) const
{
    std::scoped_lock lg(bufferMutex);
    auto bufferData = buffer[id];
    return bufferData;
}

void DataBufferStorage::releaseBuffer(uint32_t id)
{
    std::scoped_lock lg(bufferMutex);
    freeBufferIds.insert(id);
}

uint32_t DataBufferStorage::getIdDiff() const
{
    std::scoped_lock lg(bufferMutex);
    return bufferCount - freeBufferIds.size();
}
