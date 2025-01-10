#ifndef DEPTH_AWARE_SPLAY_TREE_H
#define DEPTH_AWARE_SPLAY_TREE_H

#include <bits/stdc++.h>
using namespace std;

namespace dast {

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

    int get_depth_threshold() {
        return floor(1.6 * log2(size));
    }

    Node *root = nullptr;

    // Set a new root for the tree
    Node *set_root(Node *x) {
        if (x) 
            x->parent = nullptr;
        return root = x;
    }

    // Perform a single rotation
    void rotate_up(Node *x) {
        static Node *p, *gp;
        p = x->parent;
        gp = p->parent;
        static int index;
        index = x->parent_index();

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

        threshold = get_depth_threshold();
        static Node *x;
        x = new Node();
        x->key = key;

        if (root == nullptr) {
            set_root(x);
            return;
        }

        static Node *current, *previous;
        current = root;
        previous = nullptr;
        static int depth;
        depth = 0;

        while (current != nullptr) {
            depth++;
            previous = current;
            current = current->child[current->key < x->key];
        }

        previous->set_child(int(previous->key < x->key), x);

        if (depth >= threshold)
            splay(x);
    }

    // Find the node with the smallest key >= the given key
    Node *lower_bound(const int &key) {
        Node *current;
        current = root;
        Node *answer = nullptr;answer = nullptr;
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

        if (depth >= threshold) splay(answer);
        return answer;
    }

    // Remove a specific node
    void remove(Node *x) {
        if (x == nullptr) return;

        size--;
        threshold = get_depth_threshold();
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

}
#endif