#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include "SignatureParams.h"
#include <functional>
#include <vector>
#include <mutex>

class SignatureAlgorithm
{
public:
    using FinishFunc = std::function<void()>;

    explicit SignatureAlgorithm(const SignatureParams& params);

    void start();

private:
    const SignatureParams params;
    std::vector<std::string> md5Collection;
    static std::mutex debugMutex;

    void finish(bool success = true);

    void writeDebug(const std::string& msg);
};

#endif // MAINPROCESS_H
