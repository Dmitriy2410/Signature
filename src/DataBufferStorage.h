#ifndef DATABUFFERSTORAGE_H
#define DATABUFFERSTORAGE_H

#include <mutex>
#include <vector>
#include <memory>
#include <atomic>

class DataBufferStorage {
public:
    using SingleBuffer = std::vector<char>;
    using SingleBufferPtr = std::shared_ptr<SingleBuffer>;

    explicit DataBufferStorage(uint64_t singleBufferSize, uint64_t maxBufferCount);

    std::pair<SingleBufferPtr,uint64_t> getFreeBuffer();
    SingleBufferPtr getBuffer(uint64_t id) const;

    uint64_t getIdDiff(uint64_t id) const;

private:
    const uint64_t singleBufferSize;
    const uint64_t maxBufferCount;
    std::vector<SingleBufferPtr> buffer;
    static std::mutex bufferMutex;
    std::atomic_uint64_t freeBufferId;

    void rotateFreeBufferId();
};

#endif // DATABUFFERSTORAGE_H
