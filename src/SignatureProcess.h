#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include "SignatureParams.h"
#include <functional>
#include <vector>

class SignatureProcess
{
public:
    using FinishFunc = std::function<void()>;

    explicit SignatureProcess(const SignatureParams& params, FinishFunc &&finishFunc);

    void start();

private:
    const SignatureParams params;
    const FinishFunc finishFunc;
    std::vector<std::string> md5Collection;

    void finish(bool success = true);
};

#endif // MAINPROCESS_H
