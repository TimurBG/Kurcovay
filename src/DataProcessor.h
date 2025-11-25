// DataProcessor.h
#pragma once

#include <vector>
#include <cstdint>

class DataProcessor {
public:
    static uint32_t calculateAverage(const std::vector<uint32_t>& vector);
    static uint32_t handleOverflow(uint64_t value);
};
