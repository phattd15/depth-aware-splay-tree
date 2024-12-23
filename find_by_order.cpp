#include "bits/stdc++.h"
#include "internal/original_splay_tree.h"
#include "internal/depth_aware_splay_tree.h"
#include "internal/dast_index.h"
#include "internal/test_gen.h"
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace __gnu_pbds;
using namespace std;
using namespace chrono;

typedef tree<int, null_type, less<int>, rb_tree_tag, tree_order_statistics_node_update> ordered_set;

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
    dast_index::DepthAwareSplayTree dast;
    ordered_set os;

    // Test parameters
    int numAccess = 1 << 20;
        
    vector<int> testSizes;
    for (int i = 1; i <= numAccess; i *= 2)
        testSizes.push_back(i);

    // Result storage
    vector<vector<double>> results;

    // Column headers
    vector<string> columns = {"TreeSize", "PBDS", "DepthAwareSplayTree"};

    for (int testSize : testSizes) {
        cout << "Testing tree size: " << testSize << endl;

        // Generate test data
        auto testData = test::generateTestData(testSize, numAccess);

        // Clear previous tree data
        dast.clear();
        os.clear();

        // Insert phase
        for (const auto& q : testData) {
            if (q.first == 0) {
                dast.insert(q.second);
                os.insert(q.second);
            }
        }

        // Measure time for each tree
        double pbdsResult = 0, dastResult = 0;
        auto start = high_resolution_clock::now();
        auto end = high_resolution_clock::now();
        for (const auto& q : testData) {
            if (q.first == 1) {
                // Measure PBDS
                start = high_resolution_clock::now();
                os.find_by_order(q.second);
                end = high_resolution_clock::now();
                pbdsResult += duration_cast<duration<double>>(end - start).count();

                // Measure Depth-Aware Splay Tree
                start = high_resolution_clock::now();
                dast.find_by_order(q.second);
                end = high_resolution_clock::now();
                dastResult += duration_cast<duration<double>>(end - start).count();
            }
        }

        // Average times (convert to microseconds)
        double avgPbdsTime = (pbdsResult / testData.size()) * 1e6;
        double avgDastTime = (dastResult / testData.size()) * 1e6;

        // Print results for the current cache pool size
        cout << "Test Size: " << testSize
             << ", Policy Based Data Structure: " << avgPbdsTime << "us"
             << ", Depth-Aware Splay Tree: " << avgDastTime << "us" << endl;

        // Store results for CSV
        results.push_back({(double)testSize, avgPbdsTime, avgDastTime});
    }

    // Write results to CSV
    writeCSV(results, columns, "output/find_by_order/results_log.csv");

    return 0;
}
