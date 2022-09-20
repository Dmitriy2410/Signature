#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include "SignatureParams.h"
#include <atomic>

class SignatureAlgorithm
{
public:
    explicit SignatureAlgorithm(const SignatureParams& params);

    void start();

private:
    const SignatureParams params;
    const std::atomic_bool debugMode;
    std::atomic_bool errorOccurred;
};

#endif // MAINPROCESS_H
