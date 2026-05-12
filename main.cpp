#include "TapeConfig.h"
#include "FileTape.h"    // Наш единственный бинарный класс ленты
#include "TapeSorter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <memory>
#include <stdexcept>

void textToBinary(const std::string& txtFile, const std::string& binFile) {
    std::ifstream in(txtFile);
    if (!in.is_open()) throw std::runtime_error("Failed to open input text file: " + txtFile);
    std::ofstream out(binFile, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) throw std::runtime_error("Failed to create temp binary file: " + binFile);
    int32_t value;
    while (in >> value) {
        out.write(reinterpret_cast<const char*>(&value), sizeof(value));
    }
}

void binaryToText(const std::string& binFile, const std::string& txtFile) {
    std::ifstream in(binFile, std::ios::binary);
    if (!in.is_open()) throw std::runtime_error("Failed to open temp binary file: " + binFile);
    std::ofstream out(txtFile, std::ios::trunc);
    if (!out.is_open()) throw std::runtime_error("Failed to create output text file: " + txtFile);
    int32_t value;
    while (in.read(reinterpret_cast<char*>(&value), sizeof(value))) {
        out << value << " ";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <input.txt> <output.txt> <memory_limit_bytes>\n";
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    size_t memoryLimit = std::stoull(argv[3]);

    TapeConfig config = TapeConfig::LoadFromFile("tape_config.ini");

    try {
        std::cout << "1. Preparing environment...\n";
        if (!std::filesystem::exists("tmp")) {
            std::filesystem::create_directory("tmp");
        }

        std::string tmpInputBin = "tmp/input.bin";
        std::string tmpOutputBin = "tmp/output.bin";

        std::cout << "2. Converting input text to binary format...\n";
        textToBinary(inputFile, tmpInputBin);

        {
            std::cout << "3. Starting tape merge sort...\n";

            // основные ленты
            FileTape inTape(tmpInputBin, config, false);
            FileTape outTape(tmpOutputBin, config, true);

            // временные ленты
            std::vector<std::unique_ptr<FileTape>> tmpTapesPtrs;
            std::vector<ITape*> tmpTapes;
            for (int i = 0; i < 4; ++i) {
                auto tape = std::make_unique<FileTape>("tmp/run_" + std::to_string(i) + ".bin", config, true);
                tmpTapes.push_back(tape.get());
                tmpTapesPtrs.push_back(std::move(tape));
            }

            // Запускаем сортировку (работает только с бинарниками)
            TapeSorter sorter(memoryLimit);
            sorter.sort(&inTape, &outTape, tmpTapes);
        } // Вызываются деструкторы FileTape, файлы закрываются

        std::cout << "4. Converting result back to text format...\n";
        binaryToText(tmpOutputBin, outputFile);

        std::cout << "Sorting completed successfully. Result saved to " << outputFile << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}