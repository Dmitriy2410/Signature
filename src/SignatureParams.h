#ifndef SIGNATUREPARAMS_H
#define SIGNATUREPARAMS_H

#include <string>

struct SignatureParams {
	std::string inputPath;
	std::string outputPath;
    uint64_t blockSize = 1; // bytes
};

#endif // SIGNATUREPARAMS_H
