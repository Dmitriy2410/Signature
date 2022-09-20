#ifndef DATABUFFERSTORAGE_H
#define DATABUFFERSTORAGE_H

#include <mutex>
#include <vector>
#include <memory>
#include <atomic>
#include <unordered_set>

class DataBufferStorage {
public:
    using SingleBuffer = std::vector<char>;
    using SingleBufferPtr = std::shared_ptr<SingleBuffer>;

    explicit DataBufferStorage(uint64_t singleBufferSize, uint64_t initialBufferCount);

    // Получение свободного буфера. Если все буферы заняты, количество буферов удваивается.
    std::pair<SingleBufferPtr,uint64_t> getFreeBuffer();
    // Получение буфера с данными.
    SingleBufferPtr getBuffer(uint64_t id) const;
    // Освобождение буфера, после чего буфер считается свободным и может быть получен в getFreeBuffer.
    void releaseBuffer(uint64_t id);

    uint64_t getIdDiff() const;

private:
    const uint64_t singleBufferSize;
    uint64_t bufferCount;
    std::vector<SingleBufferPtr> buffer;
    static std::mutex bufferMutex;

    std::unordered_set<uint64_t> freeBufferIds;

    void fillNewBuffers(size_t from, size_t to);
};

#endif // DATABUFFERSTORAGE_H
