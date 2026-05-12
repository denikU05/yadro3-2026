#pragma once
#include <string>
#include <fstream>
#include <sstream>

struct TapeConfig {
    int readDelayMs = 0;
    int writeDelayMs = 0;
    int shiftDelayMs = 0;
    int rewindDelayMs = 0;

    static TapeConfig LoadFromFile(const std::string& filename) {
        TapeConfig config;
        std::ifstream file(filename);
        if (!file.is_open()) {
            return config;
        }

        std::string line;
        while (std::getline(file, line)) {
            auto delimiterPos = line.find('=');
            if (delimiterPos == std::string::npos) continue;

            std::string key = line.substr(0, delimiterPos);
            std::string valueStr = line.substr(delimiterPos + 1);
            int value = std::stoi(valueStr);

            if (key == "ReadDelay") config.readDelayMs = value;
            else if (key == "WriteDelay") config.writeDelayMs = value;
            else if (key == "ShiftDelay") config.shiftDelayMs = value;
            else if (key == "RewindDelay") config.rewindDelayMs = value;
        }
        return config;
    }
};