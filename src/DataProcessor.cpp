/**
 * @file DataProcessor.cpp
 * @brief Реализация класса DataProcessor
 */

#include "DataProcessor.h"

/**
 * @brief Вычисляет среднее значение элементов вектора
 * @param vector Вектор значений
 * @return Среднее значение вектора или 0 если вектор пуст
 * 
 * Вычисляет сумму всех элементов и делит на количество элементов.
 * Использует 64-битную арифметику для избежания переполнения.
 */
uint32_t DataProcessor::calculateAverage(const std::vector<uint32_t>& vector) {
    if (vector.empty()) {
        return 0;
    }
    
    uint64_t sum = 0;
    for (uint32_t value : vector) {
        sum += value;
    }
    
    uint64_t average = sum / vector.size();
    return handleOverflow(average);
}

/**
 * @brief Обрабатывает переполнение при преобразовании типов
 * @param value Значение для преобразования
 * @return Значение, ограниченное диапазоном uint32_t
 * 
 * Если значение больше UINT32_MAX, возвращает UINT32_MAX.
 * Если значение меньше 0, возвращает 0.
 * Иначе возвращает значение, приведенное к uint32_t.
 */
uint32_t DataProcessor::handleOverflow(uint64_t value) {
    if (value > UINT32_MAX) {
        return UINT32_MAX; // 2^32 - 1
    }
    if (value < 0 ) {
        return 0;
    }
    return static_cast<uint32_t>(value);
}