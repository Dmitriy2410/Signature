#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include <functional>

class Measurement
{
public:
    static clock_t measureFunc(std::function<void()> &&func);
};

#endif //MEASUREMENT_H
