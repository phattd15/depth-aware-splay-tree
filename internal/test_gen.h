#ifndef TEST_GEN_H
#define TEST_GEN_H

#include <vector>
#include <random>
#include <unordered_set>
#include <algorithm>  // For std::shuffle
#include <utility>    // For std::pair

namespace test {
// Define TestType as a vector of pairs representing operations and values.
// Operation 0: insert, Operation 1: find
using TestType = std::vector<std::pair<int, int>>;

TestType generateTestData(int testSize, int numAccess) {
    TestType testData;
    std::mt19937 gen(0);  // Fixed seed for reproducibility
    std::uniform_int_distribution<> dist(0, testSize - 1);

    // Generate all insert operations in a sequential order and then shuffle
    for (int i = 0; i < testSize; i++) {
        testData.emplace_back(0, i);  // Operation 0: insert
    }
    std::shuffle(testData.begin(), testData.end(), gen);  // Shuffle insertions

    // Generate find operations
    for (int i = 0; i < numAccess; i++) {
        int value = dist(gen);
        testData.emplace_back(1, value);  // Operation 1: find
    }

    return testData;
}

TestType generateGradualAccessTest(int testSize, int accessCycles) {
    TestType testData;
    std::mt19937 gen(0);  // Fixed seed

    // Insert all elements in random order initially
    for (int i = 0; i < testSize; i++) {
        testData.emplace_back(0, i);  // Operation 0: insert
    }
    std::shuffle(testData.begin(), testData.end(), gen);  // Shuffle insertions

    // Generate repeated find operations in sequential order across cycles
    for (int cycle = 0; cycle < accessCycles; cycle++) {
        for (int i = 0; i < testSize; i++) {
            testData.emplace_back(1, i);  // Operation 1: find
        }
    }

    return testData;
}

TestType generateCacheAccessTest(int testSize, int subsetSize, int numAccess) {
    TestType testData;
    std::mt19937 gen(0);  // Fixed seed
    std::uniform_int_distribution<> dist(0, testSize - 1);

    // Insert all elements in random order initially
    for (int i = 0; i < testSize; i++) {
        testData.emplace_back(0, i);  // Operation 0: insert
    }
    std::shuffle(testData.begin(), testData.end(), gen);  // Shuffle insertions

    // Create a subset of unique elements to be accessed repeatedly
    std::unordered_set<int> uniqueSubset;
    while (uniqueSubset.size() < subsetSize) {
        uniqueSubset.insert(dist(gen));
    }

    // Convert the unique subset to a vector for random access
    std::vector<int> subsetVector(uniqueSubset.begin(), uniqueSubset.end());

    // Perform repeated find operations on the subset elements
    for (int i = 0; i < numAccess; i++) {
        int value = subsetVector[dist(gen) % subsetSize];
        testData.emplace_back(1, value);  // Operation 1: find
    }

    return testData;
}

}
#endif // TEST_GEN_H
