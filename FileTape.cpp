#include "FileTape.h"
#include <thread>
#include <chrono>
#include <stdexcept>
#include <utility>

FileTape::FileTape(std::string filepath, const TapeConfig& config, bool create_new)
    : filepath_(std::move(filepath)), config_(config) {
    openFile(create_new);
}

FileTape::~FileTape() {
    if (file_.is_open()) file_.close();
}

void FileTape::openFile(bool truncate) {
    if (file_.is_open()) file_.close();

    // Бинарный режим
    auto flags = std::ios::binary | std::ios::in | std::ios::out;
    if (truncate) {
        flags |= std::ios::trunc;
    }

    file_.open(filepath_, flags);
    if (!file_.is_open()) {
        std::ofstream create_file(filepath_, std::ios::binary | std::ios::trunc);
        create_file.close();
        file_.open(filepath_, flags);
    }

    if (!file_.is_open()) {
        throw std::runtime_error("Failed to open binary tape file: " + filepath_);
    }

    // Вычисляем размер файла
    file_.seekg(0, std::ios::end);
    fileSize_ = file_.tellg();

    // Возвращаемся в начало
    file_.seekg(0, std::ios::beg);
    file_.seekp(0, std::ios::beg);
    currentPos_ = 0;
}

void FileTape::delay(int ms) {
    if (ms > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    }
}

int32_t FileTape::read() {
    if (isEnd()) throw std::out_of_range("Reading past the end of the binary tape");
    delay(config_.readDelayMs);

    int32_t value;
    file_.read(reinterpret_cast<char*>(&value), sizeof(value));
    currentPos_ += sizeof(value);
    return value;
}

void FileTape::write(int32_t value) {
    delay(config_.writeDelayMs);
    file_.write(reinterpret_cast<const char*>(&value), sizeof(value));
    currentPos_ += sizeof(value);

    if (currentPos_ > fileSize_) {
        fileSize_ = currentPos_; // Обновляем размер файла, если дописали в конец
    }
}

void FileTape::moveForward() {
    delay(config_.shiftDelayMs);
    if (currentPos_ + sizeof(int32_t) <= fileSize_) {
        currentPos_ += sizeof(int32_t);
        file_.seekp(currentPos_);
        file_.seekg(currentPos_);
    }
}

void FileTape::moveBackward() {
    delay(config_.shiftDelayMs);
    // Идеальный шаг назад - просто отступаем на 4 байта
    if (currentPos_ >= sizeof(int32_t)) {
        currentPos_ -= sizeof(int32_t);
        file_.seekp(currentPos_);
        file_.seekg(currentPos_);
    }
}

void FileTape::rewind() {
    delay(config_.rewindDelayMs);
    file_.clear();
    currentPos_ = 0;
    file_.seekp(0, std::ios::beg);
    file_.seekg(0, std::ios::beg);
}

bool FileTape::isEnd() {
    return currentPos_ >= fileSize_;
}

void FileTape::clear() {
    openFile(true);
}