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
            file << row[i] * 1000;  // Convert to milliseconds (multiply by 1000)
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

    int testSize = 100000;
    int cycle = 10;

    // Generate test data
    auto testData = test::generateGradualAccessTest(testSize, cycle);

    vector<vector<double>> normalSplayTimes;
    // Benchmark fast splay tree
    for (auto q : testData) {
        if (q.first == 0) {
            tree.insert(q.second);
        } else {
            auto start = high_resolution_clock::now();
            tree.lower_bound(q.second);
            auto end = high_resolution_clock::now();
            double queryTime = duration_cast<duration<double>>(end - start).count();
            normalSplayTimes.push_back({queryTime});
        }
    }

    vector<vector<double>> fastSplayTimes;

    // Benchmark fast splay tree
    for (auto q : testData) {
        if (q.first == 0) {
            dastTree.insert(q.second);
        } else {
            auto start = high_resolution_clock::now();
            dastTree.lower_bound(q.second);
            auto end = high_resolution_clock::now();
            double queryTime = duration_cast<duration<double>>(end - start).count();
            fastSplayTimes.push_back({queryTime});
        }
    }

    // Write results to CSV files in the "output" directory
    writeCSV(normalSplayTimes, {"query_time"}, "output/worst_case_experiment/splay_tree.csv");
    writeCSV(fastSplayTimes, {"query_time"}, "output/worst_case_experiment/dast_tree.csv");

    return 0;
}
