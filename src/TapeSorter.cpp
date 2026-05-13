#include "TapeSorter.h"
#include <vector>
#include <algorithm>
#include <stdexcept>

TapeSorter::TapeSorter(size_t memoryLimitBytes)
    : memoryLimitBytes_(memoryLimitBytes) {}

void TapeSorter::sort(ITape* inputTape, ITape* outputTape, const std::vector<ITape*>& tmpTapes) const {
    if (tmpTapes.size() != 4) {
        throw std::invalid_argument("TapeSorter requires exactly 4 temporary tapes.");
    }

    size_t capacity = memoryLimitBytes_ / sizeof(int32_t);
    if (capacity == 0) {
        throw std::invalid_argument("Memory limit is too small to hold even one element.");
    }

    // Распределение
    inputTape->rewind();
    std::vector<int32_t> buffer;
    buffer.reserve(capacity);
    size_t totalRuns = 0;
    int writeIdx = 0; // Индекс временной ленты для записи (0 или 1)

    while (!inputTape->isEnd()) {
        buffer.clear();
        while (!inputTape->isEnd() && buffer.size() < capacity) {
            buffer.push_back(inputTape->read());
        }

        if (buffer.empty()) break;

        // Сортируем
        std::sort(buffer.begin(), buffer.end());

        // Записываем отсортированный блок на одну из стартовых временных лент
        for (int32_t val : buffer) {
            tmpTapes[writeIdx]->write(val);
        }

        writeIdx = 1 - writeIdx; // Чередуем запись
        totalRuns++;
    }

    // Если входной файл был пуст
    if (totalRuns == 0) return;

    // Слияние
    int inStart = 0;  // Индексы лент для чтения (0 и 1)
    int outStart = 2; // Индексы лент для записи (2 и 3)
    size_t currentRunSize = capacity;

    while (totalRuns > 1) {
        for (int i = 0; i < 4; ++i) {
            tmpTapes[i]->rewind();
        }
        tmpTapes[outStart]->clear();
        tmpTapes[outStart + 1]->clear();

        size_t runsWritten = 0;
        int currentOutIdx = outStart;

        ITape* t1 = tmpTapes[inStart];
        ITape* t2 = tmpTapes[inStart + 1];

        while (!t1->isEnd() || !t2->isEnd()) {
            size_t read1 = 0, read2 = 0;
            ITape* outTape = tmpTapes[currentOutIdx];

            bool has1 = !t1->isEnd() && read1 < currentRunSize;
            bool has2 = !t2->isEnd() && read2 < currentRunSize;

            int32_t val1 = has1 ? t1->read() : 0;
            int32_t val2 = has2 ? t2->read() : 0;

            // Слияние двух отсортированных последовательностей
            while (has1 || has2) {
                if (has1 && has2) {
                    if (val1 <= val2) {
                        outTape->write(val1);
                        read1++;
                        if (read1 < currentRunSize && !t1->isEnd()) val1 = t1->read();
                        else has1 = false;
                    } else {
                        outTape->write(val2);
                        read2++;
                        if (read2 < currentRunSize && !t2->isEnd()) val2 = t2->read();
                        else has2 = false;
                    }
                } else if (has1) {
                    outTape->write(val1);
                    read1++;
                    if (read1 < currentRunSize && !t1->isEnd()) val1 = t1->read();
                    else has1 = false;
                } else if (has2) {
                    outTape->write(val2);
                    read2++;
                    if (read2 < currentRunSize && !t2->isEnd()) val2 = t2->read();
                    else has2 = false;
                }
            }

            // Меняем целевую ленту для следующей пары отрезков
            currentOutIdx = currentOutIdx == outStart ? outStart + 1 : outStart;
            runsWritten++;
        }

        currentRunSize *= 2;
        totalRuns = runsWritten;
        std::swap(inStart, outStart); // Меняем роли лент местами
    }

    // Копирование результата на выходную ленту
    ITape* finalTape = tmpTapes[inStart];
    finalTape->rewind();

    outputTape->rewind();
    outputTape->clear();

    while (!finalTape->isEnd()) {
        outputTape->write(finalTape->read());
    }
}
