#include "bits/stdc++.h"
#include "internal/test_gen.h"

using namespace std;
using namespace chrono;

// Node structure for the Splay Tree
struct Node {
    Node *parent = nullptr;
    Node *child[2] = {nullptr, nullptr};
    int key;

    void set_child(int index, Node *child_node) {
        child[index] = child_node;
        if (child_node)
            child_node->parent = this;
    }

    int parent_index() const {
        return parent == nullptr ? -1 : int(this == parent->child[1]);
    }

    ~Node() {
        delete child[0];
        delete child[1];
    }
};

struct DepthAwareSplayTree {
    int size = 0;
    int threshold = 0;
    int total_depth = 0;
    int splay_count = 0;

    Node *root = nullptr;

    Node *set_root(Node *x) {
        if (x)
            x->parent = nullptr;
        return root = x;
    }

    void rotate_up(Node *x) {
        Node *p = x->parent;
        Node *gp = p->parent;
        int index = x->parent_index();

        if (gp) {
            gp->set_child(p->parent_index(), x);
        } else {
            set_root(x);
        }

        p->set_child(index, x->child[!index]);
        x->set_child(!index, p);
    }

    void splay(Node *x) {
        while (x != root) {
            if (x->parent != root)
                rotate_up(x->parent_index() == x->parent->parent_index() ? x->parent : x);
            rotate_up(x);
        }
    }

    void insert(int key) {
        size++;

        Node *x = new Node();
        x->key = key;

        if (root == nullptr) {
            set_root(x);
            return;
        }

        Node *current = root;
        Node *previous = nullptr;
        int depth = 0;

        while (current != nullptr) {
            depth++;
            previous = current;
            current = current->child[current->key < x->key];
        }

        previous->set_child(int(previous->key < x->key), x);

        if (depth >= threshold)
            splay(x);
    }

    Node *lower_bound(const int &key) {
        Node *current = root;
        Node *answer = nullptr;
        int depth = 0;

        while (current != nullptr) {
            depth++;

            if (current->key < key) {
                current = current->child[1];
            } else {
                answer = current;
                current = current->child[0];
            }
        }

        total_depth += depth;
        if (depth >= threshold) {
            splay_count++;
            splay(answer);
        }
        return answer;
    }

    void remove(Node *x) {
        if (x == nullptr)
            return;

        size--;

        splay(x);
        if (x->child[0])
            x->child[0]->parent = nullptr;
        if (x->child[1])
            x->child[1]->parent = nullptr;

        Node *left_subtree = x->child[0];
        Node *right_subtree = x->child[1];

        delete x;

        if (!left_subtree) {
            set_root(right_subtree);
        } else {
            Node *max_left = left_subtree;
            while (max_left->child[1]) {
                max_left = max_left->child[1];
            }
            splay(max_left);
            max_left->set_child(1, right_subtree);
            set_root(max_left);
        }
    }

    void clear() {
        delete root;
        root = nullptr;
        size = 0;
    }

    ~DepthAwareSplayTree() {
        clear();
    }
};

int main() {
    vector<int> testSizes = {10, 100, 1000, 10000, 100000};
    int numAccess = 100000;

    for (int testSize : testSizes) {
        string outputFileName = "output/dast_analysis/dast_data_" + to_string(testSize) + ".csv";
        auto testData = test::generateTestData(testSize, numAccess);

        // Open the CSV file for writing
        std::ofstream outFile(outputFileName);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open file for writing!" << std::endl;
            return 1;
        }

        // Write the CSV headers
        outFile << "depth_threshold,avg_query_time(microseconds),avg_depth,splay_count\n";

        for (int threshold = 0; threshold < 120; threshold++) {
            DepthAwareSplayTree tree;
            tree.threshold = threshold;
            double total_query_time = 0.0;

            for (auto q : testData) {
                if (q.first == 0) {
                    tree.insert(q.second);
                } else {
                    auto start = std::chrono::high_resolution_clock::now();
                    auto it = tree.lower_bound(q.second);
                    auto end = std::chrono::high_resolution_clock::now();

                    double queryTime = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
                    total_query_time += queryTime;
                }
            }

            total_query_time *= 1'000'000;
            total_query_time /= numAccess;

            double avg_depth = tree.total_depth;
            avg_depth /= numAccess;

            outFile << threshold << ","
                    << std::fixed << std::setprecision(6) << total_query_time << ","
                    << std::fixed << std::setprecision(6) << avg_depth << ","
                    << tree.splay_count << "\n";

            cerr << threshold << ","
                 << std::fixed << std::setprecision(6) << total_query_time << ","
                 << std::fixed << std::setprecision(6) << avg_depth << ","
                 << tree.splay_count << "\n";
        }

        outFile.close();
        std::cout << "Data written to " << outputFileName << std::endl;
    }

    return 0;
}
