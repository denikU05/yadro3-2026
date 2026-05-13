#pragma once
#include "ITape.h"
#include "TapeConfig.h"
#include <fstream>
#include <string>

class FileTape : public ITape {
public:
    FileTape(std::string  filepath, const TapeConfig& config, bool create_new = false);
    ~FileTape() override;

    int32_t read() override;
    void write(int32_t value) override;
    void moveForward() override;
    void moveBackward() override;
    void rewind() override;
    bool isEnd() override;
    void clear() override;

private:
    std::string filepath_;
    TapeConfig config_;
    std::fstream file_;

    size_t currentPos_ = 0;
    size_t fileSize_ = 0;

    static void delay(int ms);
    void openFile(bool truncate);
};