#include "bits/stdc++.h"
#include "internal/original_splay_tree.h"
#include "internal/depth_aware_splay_tree.h"
#include "internal/sum_query_dast.h"
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
    sum_query_dast::DepthAwareSplayTree dastTree;
    ost::SplayTree tree;
    set<int> stdSet;

    // Test parameters
    int numAccess = 100;
        
    vector<int> testSizes;
    for (int i = 1; i <= (1 << 20); i *= 2)
        testSizes.push_back(i);

    // Result storage
    vector<vector<double>> results;

    // Column headers
    vector<string> columns = {"TreeSize", "std::set", "DepthAwareSplayTree"};

    random_device rd;  // Seed generator
    mt19937 gen(rd()); // Mersenne Twister PRNG

    for (int testSize : testSizes) {
        cout << "Testing tree size: " << testSize << endl;
        uniform_int_distribution<int> dist(0, testSize - 1);

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
        auto set_sum = [&](int nl, int nr) -> long long {
            long long sum = 0;

            // Get iterator to the first element that is >= nl
            auto it = stdSet.lower_bound(nl);

            // Iterate while within range and not past nr
            while (it != stdSet.end() && *it <= nr) {
                sum += *it;
                ++it;
            }

            return sum;
        };

        auto dast_sum = [&](int nl, int nr) -> long long {
            auto node_right = dastTree.lower_bound(nr);
            auto node_left = dastTree.lower_bound(nl);

            return dastTree.range_sum(node_left, node_right);
        };

        for (int i = 0; i < numAccess; i++) {
            int n1 = dist(gen);
            int n2 = dist(gen);

            if (n2 < n1) swap(n1, n2);

            // benchmark n1 n2

            auto start = high_resolution_clock::now();
            set_sum(n1, n2);
            auto end = high_resolution_clock::now();
            stdSetResult += duration_cast<duration<double>>(end - start).count();

            // Measure Depth-Aware Splay Tree
            start = high_resolution_clock::now();
            dast_sum(n1, n2);
            end = high_resolution_clock::now();
            dastResult += duration_cast<duration<double>>(end - start).count();
        }

        // Average times (convert to microseconds)
        double avgStdSetTime = (stdSetResult / numAccess) * 1e6;
        double avgDastTime = (dastResult / numAccess) * 1e6;

        // Print results for the current cache pool size
        cout << "Test Size: " << testSize
             << ", std::set: " << avgStdSetTime << "us"
             << ", Depth-Aware Splay Tree: " << avgDastTime << "us" << endl;

        // Store results for CSV
        results.push_back({(double)testSize, avgStdSetTime, avgDastTime});
    }

    // Write results to CSV
    writeCSV(results, columns, "output/range_sum_benchmark/results_log.csv");

    return 0;
}
