#ifndef SIGNATUREPARAMS_H
#define SIGNATUREPARAMS_H

#include <string>

struct SignatureParams {
    std::string inputPath;
    std::string outputPath;
    uint32_t blockSize = 1024 * 1024; // bytes

    // settings
    bool debugMode = false;
    int threadCount = 15;
};

#endif // SIGNATUREPARAMS_H
