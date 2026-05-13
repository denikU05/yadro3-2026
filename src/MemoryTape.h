#pragma once
#include "ITape.h"
#include <vector>
#include <stdexcept>

class MemoryTape : public ITape {
public:
    explicit MemoryTape(const std::vector<int32_t>& initialData = {})
        : data_(initialData), pos_(0) {}

    int32_t read() override {
        if (isEnd()) throw std::out_of_range("Read past end");
        return data_[pos_++];
    }

    void write(int32_t value) override {
        if (pos_ < data_.size()) {
            data_[pos_] = value;
        } else {
            data_.push_back(value);
        }
        pos_++;
    }

    void moveForward() override {
        if (pos_ < data_.size()) pos_++;
    }

    void moveBackward() override {
        if (pos_ > 0) pos_--;
    }

    void rewind() override {
        pos_ = 0;
    }

    bool isEnd() override {
        return pos_ >= data_.size();
    }

    void clear() override {
        data_.clear();
        pos_ = 0;
    }

    [[nodiscard]] const std::vector<int32_t>& getData() const { return data_; }

private:
    std::vector<int32_t> data_;
    size_t pos_;
};