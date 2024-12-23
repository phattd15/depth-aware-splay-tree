#include "bits/stdc++.h"
#include "internal/test_gen.h"

using namespace std;
using namespace chrono;

// Node structure for the Splay Tree
struct Node {
    Node *parent = nullptr;
    Node *child[2] = {nullptr, nullptr};
    int key;

    // Set a child node and update its parent pointer
    void set_child(int index, Node *child_node) {
        child[index] = child_node;
        if (child_node)
            child_node->parent = this;
    }

    // Determine the index of this node relative to its parent
    int parent_index() const {
        return parent == nullptr ? -1 : int(this == parent->child[1]);
    }

    ~Node() {
        // Recursively delete children
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

    // Set a new root for the tree
    Node *set_root(Node *x) {
        if (x) 
            x->parent = nullptr;
        return root = x;
    }

    // Perform a single rotation
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

    // Splay operation to move a node to the root
    void splay(Node *x) {
        while (x != root) {
            if (x->parent != root)
                rotate_up(x->parent_index() == x->parent->parent_index() ? x->parent : x);
            rotate_up(x);
        }
    }

    // Insert a key into the tree
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

        if (depth > threshold)
            splay(x);
    }

    // Find the node with the smallest key >= the given key
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
        if (answer && depth >= threshold) {
            splay_count++;
            splay(answer);
        }
        return answer;
    }

    // Remove a specific node
    void remove(Node *x) {
        if (x == nullptr) return;

        size--;

        splay(x); // Bring x to the root
        if (x->child[0]) x->child[0]->parent = nullptr;
        if (x->child[1]) x->child[1]->parent = nullptr;

        Node *left_subtree = x->child[0];
        Node *right_subtree = x->child[1];

        delete x; // Free memory for the node

        if (!left_subtree) {
            set_root(right_subtree);
        } else {
            Node *max_left = left_subtree;
            while (max_left->child[1]) {
                max_left = max_left->child[1];
            }
            splay(max_left); // Bring max of left subtree to the root
            max_left->set_child(1, right_subtree);
            set_root(max_left);
        }
    }

    // Clear the entire tree
    void clear() {
        delete root;
        root = nullptr;
        size = 0;
    }

    // Destructor to clear the tree when it goes out of scope
    ~DepthAwareSplayTree() {
        clear();
    }
};

int main() {
    int testSize = 10000;
    int cycles = 10;
    int numAccess = testSize * cycles;
    auto testData = test::generateGradualAccessTest(testSize, cycles);

    // Open the CSV file for writing
    std::ofstream outFile("output/dast_analysis/adversarial_dast_data.csv");
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open file for writing!" << std::endl;
        return 1;
    }

    // Write the CSV headers
    outFile << "depth_threshold,avg_query_time(microseconds),avg_depth,splay_count\n";

    for (int threshold = 0; threshold < 300; threshold++) {
        DepthAwareSplayTree tree;
        tree.threshold = threshold;
        double total_query_time = 0.0;

        for (auto q : testData) {
            if (q.first == 0) {
                tree.insert(q.second);
            } else {
                auto start = std::chrono::high_resolution_clock::now();
                tree.lower_bound(q.second);
                auto end = std::chrono::high_resolution_clock::now();
                double queryTime = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();
                total_query_time += queryTime;
            }
        }

        // Calculate average query time in microseconds
        total_query_time *= 1'000'000;
        total_query_time /= numAccess;

        // Calculate average depth
        double avg_depth = tree.total_depth;
        avg_depth /= numAccess;

        // Write data to the CSV file
        outFile << threshold << ","
                << std::fixed << std::setprecision(6) << total_query_time << ","
                << std::fixed << std::setprecision(6) << avg_depth << ","
                << tree.splay_count << "\n";

        cerr << threshold << ","
                << std::fixed << std::setprecision(6) << total_query_time << ","
                << std::fixed << std::setprecision(6) << avg_depth << ","
                << tree.splay_count << "\n";
    }

    // Close the file
    outFile.close();

    std::cout << "Data written to output/dast_analysis/dast_data.csv" << std::endl;

    return 0;
}
