#ifndef DATABUFFERSTORAGE_H
#define DATABUFFERSTORAGE_H

#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>

class DataBufferStorage
{
public:
    using SingleBuffer = std::vector<char>;
    using SingleBufferPtr = std::shared_ptr<SingleBuffer>;

    explicit DataBufferStorage(uint32_t singleBufferSize, uint32_t initialBufferCount);

    // Получение свободного буфера. Если все буферы заняты, количество буферов удваивается.
    std::pair<SingleBufferPtr, uint32_t> getFreeBuffer();

    // Получение буфера с данными.
    SingleBufferPtr getBuffer(uint32_t id) const;

    // Освобождение буфера, после чего буфер считается свободным и может быть получен в getFreeBuffer.
    void releaseBuffer(uint32_t id);

    uint32_t getIdDiff() const;

private:
    const uint32_t singleBufferSize;
    uint32_t bufferCount;
    std::vector<SingleBufferPtr> buffer;
    static std::mutex bufferMutex;

    std::unordered_set<uint32_t> freeBufferIds;

    void fillNewBuffers(size_t from, size_t to);
};

#endif // DATABUFFERSTORAGE_H
