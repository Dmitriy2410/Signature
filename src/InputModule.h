#ifndef INPUTMODULE_H
#define INPUTMODULE_H

#include <fstream>

class InputModule
{
public:
    InputModule();
    bool init(const std::string &filePath);
    uint32_t read(char *buffer, uint32_t blockSize);
    bool eof();
    void close();

    bool isErrorOccurred() const;

private:
    std::ifstream inputFile;
    bool errorOccurred;
};

#endif //INPUTMODULE_H
