#pragma once
#include <cstdint>

class ITape {
public:
    virtual ~ITape() = default;

    virtual int32_t read() = 0;
    virtual void write(int32_t value) = 0;
    virtual void moveForward() = 0;
    virtual void moveBackward() = 0;
    virtual void rewind() = 0;
    virtual bool isEnd() = 0;
    virtual void clear() = 0;
};