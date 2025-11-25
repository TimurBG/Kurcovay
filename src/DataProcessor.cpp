// DataProcessor.cpp
#include "DataProcessor.h"

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

uint32_t DataProcessor::handleOverflow(uint64_t value) {
    if (value > UINT32_MAX) {
        return UINT32_MAX; // 2^32 - 1
    }
    if (value < 0 ) {
        return 0;
    }
    return static_cast<uint32_t>(value);
}
