#include <UnitTest++/UnitTest++.h>
#include "DataProcessor.h"
#include <vector>
#include <climits>

SUITE(DataProcessorTests) {
    TEST(CalculateAverageNormal) {
        std::vector<uint32_t> data = {1, 2, 3, 4, 5};
        uint32_t result = DataProcessor::calculateAverage(data);
        CHECK_EQUAL(3, result);
    }
    
    TEST(CalculateAverageEmpty) {
        std::vector<uint32_t> data;
        uint32_t result = DataProcessor::calculateAverage(data);
        CHECK_EQUAL(0, result);
    }
    
    TEST(CalculateAverageSingle) {
        std::vector<uint32_t> data = {42};
        uint32_t result = DataProcessor::calculateAverage(data);
        CHECK_EQUAL(42, result);
    }
    
    TEST(CalculateAverageLargeNumbers) {
        std::vector<uint32_t> data = {1000000, 2000000, 3000000};
        uint32_t result = DataProcessor::calculateAverage(data);
        CHECK_EQUAL(2000000, result);
    }
    
    TEST(CalculateAverageRoundsDown) {
        std::vector<uint32_t> data = {1, 2, 3, 4};
        uint32_t result = DataProcessor::calculateAverage(data);
        CHECK_EQUAL(2, result); // (1+2+3+4)/4 = 2.5 → округляется до 2
    }
    
    TEST(CalculateAverageOverflowInSum) {
        std::vector<uint32_t> data = {UINT32_MAX, UINT32_MAX};
        uint32_t result = DataProcessor::calculateAverage(data);
        CHECK_EQUAL(UINT32_MAX, result);
    }
    
    TEST(HandleOverflowNormal) {
        uint32_t result = DataProcessor::handleOverflow(100);
        CHECK_EQUAL(100, result);
    }
    
    TEST(HandleOverflowMax) {
        uint32_t result = DataProcessor::handleOverflow(UINT32_MAX);
        CHECK_EQUAL(UINT32_MAX, result);
    }
    
    TEST(HandleOverflowGreaterThanMax) {
        uint64_t largeValue = static_cast<uint64_t>(UINT32_MAX) + 1;
        uint32_t result = DataProcessor::handleOverflow(largeValue);
        CHECK_EQUAL(UINT32_MAX, result);
    }
    
    TEST(HandleOverflowMuchGreaterThanMax) {
        uint64_t largeValue = static_cast<uint64_t>(UINT32_MAX) * 2;
        uint32_t result = DataProcessor::handleOverflow(largeValue);
        CHECK_EQUAL(UINT32_MAX, result);
    }
    
}