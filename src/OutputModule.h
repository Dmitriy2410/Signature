#ifndef OUTPUTMODULE_H
#define OUTPUTMODULE_H

#include <string>
#include <fstream>
#include <mutex>
#include <map>
#include <atomic>

class OutputModule
{
public:
    OutputModule(bool debugMode = false);
    bool init(const std::string& outputPath);

    void writeStr(uint64_t bufferId, const std::string& str);

    void close();

private:
    const std::atomic_bool debugMode;
    std::ofstream outputFile;
    uint64_t expectedId;
    static std::mutex outputMutex;
    std::map<uint64_t, std::string> outOfOrderStrings;

    void writeOutput(const std::string &str);

    void checkOutOfOrderStrings();
};

#endif // OUTPUTMODULE_H
