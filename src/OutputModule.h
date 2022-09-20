#ifndef OUTPUTMODULE_H
#define OUTPUTMODULE_H

#include <fstream>
#include <map>
#include <mutex>
#include <string>

class OutputModule
{
public:
    OutputModule();

    bool init(const std::string &outputPath);

    void writeStr(uint64_t bufferId, const std::string &str);

    void close();

    size_t getMaxMapSize() const;

    bool isErrorOccurred() const;

private:
    std::ofstream outputFile;
    uint64_t expectedId;
    static std::mutex outputMutex;
    std::map<uint64_t, std::string> outOfOrderStrings;
    size_t maxMapSize;
    bool errorOccurred;

    void writeOutput(const std::string &str);

    void checkOutOfOrderStrings();
};

#endif // OUTPUTMODULE_H
