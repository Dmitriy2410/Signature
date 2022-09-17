#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include "SignatureParams.h"
#include <functional>
#include <vector>
#include <mutex>
#include <atomic>

class SignatureAlgorithm
{
public:
    using FinishFunc = std::function<void()>;

    explicit SignatureAlgorithm(const SignatureParams& params);

    void start();

private:
    const SignatureParams params;
    const std::atomic_bool debugMode;
    std::vector<std::string> md5Collection;
    std::atomic_bool errorOccured;

    void finish(bool success = true);
};

#endif // MAINPROCESS_H
