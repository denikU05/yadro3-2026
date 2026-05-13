#pragma once
#include "ITape.h"
#include <vector>
#include <cstddef>

class TapeSorter {
public:
    explicit TapeSorter(size_t memoryLimitBytes);

    void sort(ITape* inputTape, ITape* outputTape, const std::vector<ITape*>& tmpTapes) const;

private:
    size_t memoryLimitBytes_;
};