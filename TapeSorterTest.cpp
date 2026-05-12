#include <gtest/gtest.h>
#include "TapeSorter.h"
#include "MemoryTape.h"

class TapeSorterTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем 4 временные ленты для каждого теста
        for (int i = 0; i < 4; ++i) {
            tmpTapes.push_back(new MemoryTape());
        }
    }

    void TearDown() override {
        for (auto t : tmpTapes) delete t;
        tmpTapes.clear();
    }

    std::vector<ITape*> tmpTapes;
};

// Тест 1: Сортировка обычного массива
TEST_F(TapeSorterTest, BasicSorting) {
    MemoryTape input({5, 3, 8, 1, 9, 2});
    MemoryTape output;
    
    // Лимит памяти 8 байт (вмещает 2 числа) - заставит алгоритм делать слияния
    TapeSorter sorter(8); 
    sorter.sort(&input, &output, tmpTapes);

    std::vector expected = {1, 2, 3, 5, 8, 9};
    EXPECT_EQ(output.getData(), expected);
}

// Тест 2: Пустая лента
TEST_F(TapeSorterTest, EmptyTape) {
    MemoryTape input;
    MemoryTape output;
    
    TapeSorter sorter(1024);
    sorter.sort(&input, &output, tmpTapes);

    EXPECT_TRUE(output.getData().empty());
}

// Тест 3: Лента из одного элемента
TEST_F(TapeSorterTest, SingleElement) {
    MemoryTape input({42});
    MemoryTape output;
    
    TapeSorter sorter(1024);
    sorter.sort(&input, &output, tmpTapes);

    std::vector expected = {42};
    EXPECT_EQ(output.getData(), expected);
}

// Тест 4: Уже отсортированная лента
TEST_F(TapeSorterTest, AlreadySorted) {
    std::vector data = {1, 2, 3, 4, 5};
    MemoryTape input(data);
    MemoryTape output;
    
    TapeSorter sorter(8);
    sorter.sort(&input, &output, tmpTapes);

    EXPECT_EQ(output.getData(), data);
}

// Тест 5: Обратно отсортированная лента
TEST_F(TapeSorterTest, ReverseSorted) {
    MemoryTape input({5, 4, 3, 2, 1});
    MemoryTape output;
    
    TapeSorter sorter(4); // Минимальный лимит памяти (1 число)
    sorter.sort(&input, &output, tmpTapes);

    std::vector expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(output.getData(), expected);
}

// Тест 6: Дубликаты
TEST_F(TapeSorterTest, Duplicates) {
    MemoryTape input({2, 1, 2, 1, 3, 1});
    MemoryTape output;
    
    TapeSorter sorter(8);
    sorter.sort(&input, &output, tmpTapes);

    std::vector expected = {1, 1, 1, 2, 2, 3};
    EXPECT_EQ(output.getData(), expected);
}