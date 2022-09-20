#include "Measurement.h"

clock_t Measurement::measureFunc(std::function<void()> &&func)
{
    auto begTime = clock();
    if (func) {
        func();
    }
    return clock() - begTime;
}
