#ifndef SIGNATUREPARAMS_H
#define SIGNATUREPARAMS_H

#include <string>

struct SignatureParams {    
    std::string inputPath;
    std::string outputPath;
    uint64_t blockSize = 1024 * 1024; // bytes

    // settings
    bool debugMode = false;
};

#endif // SIGNATUREPARAMS_H
