#include "bits/stdc++.h"
#include "internal/original_splay_tree.h"
#include "internal/depth_aware_splay_tree.h"
#include "internal/test_gen.h"

using namespace std;
using namespace chrono;

// Function to write data to CSV
void writeCSV(const vector<vector<double>>& data, const vector<string>& columns, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }
    file << fixed << setprecision(6);  // Set precision for float/double values

    // Write column headers
    for (size_t i = 0; i < columns.size(); ++i) {
        file << columns[i];
        if (i < columns.size() - 1) {
            file << ",";
        }
    }
    file << "\n";

    // Write data rows
    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];  // Write the value as milliseconds
            if (i < row.size() - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();
    cout << "Data has been written to " << filename << endl;
}

int main() {
    dast::DepthAwareSplayTree dastTree;
    ost::SplayTree tree;
    set<int> stdSet;

    // Test parameters
    int numAccess = 2000000;
    vector<int> testSizes = {10, 100, 1000, 10000, 100000, 1000000};

    // Result storage
    vector<vector<double>> results;

    // Column headers
    vector<string> columns = {"TreeSize", "std::set", "OriginalSplayTree", "DepthAwareSplayTree"};

    for (int testSize : testSizes) {
        cout << "Testing tree size: " << testSize << endl;

        // Generate test data
        auto testData = test::generateTestData(testSize, numAccess);

        // Clear previous tree data
        dastTree.clear();
        tree.clear();
        stdSet.clear();

        // Insert phase
        for (const auto& q : testData) {
            if (q.first == 0) {
                dastTree.insert(q.second);
                tree.insert(q.second);
                stdSet.insert(q.second);
            }
        }

        // Measure time for each tree
        double stdSetResult = 0, treeResult = 0, dastResult = 0;

        for (const auto& q : testData) {
            if (q.first == 1) {
                // Measure std::set
                auto start = high_resolution_clock::now();
                stdSet.find(q.second);
                auto end = high_resolution_clock::now();
                stdSetResult += duration_cast<duration<double>>(end - start).count();

                // Measure Original Splay Tree
                start = high_resolution_clock::now();
                tree.lower_bound(q.second);
                end = high_resolution_clock::now();
                treeResult += duration_cast<duration<double>>(end - start).count();

                // Measure Depth-Aware Splay Tree
                start = high_resolution_clock::now();
                dastTree.lower_bound(q.second);
                end = high_resolution_clock::now();
                dastResult += duration_cast<duration<double>>(end - start).count();
            }
        }

        // Average times (convert to microseconds)
        double avgStdSetTime = (stdSetResult / testData.size()) * 1e6;
        double avgTreeTime = (treeResult / testData.size()) * 1e6;
        double avgDastTime = (dastResult / testData.size()) * 1e6;

        // Print results for the current cache pool size
        cout << "Test Size: " << testSize
             << ", std::set: " << avgStdSetTime << "us"
             << ", Original Splay Tree: " << avgTreeTime << "us"
             << ", Depth-Aware Splay Tree: " << avgDastTime << "us" << endl;

        // Store results for CSV
        results.push_back({(double)testSize, avgStdSetTime, avgTreeTime, avgDastTime});
    }

    // Write results to CSV
    writeCSV(results, columns, "output/random_benchmark/results_o2.csv");

    return 0;
}
