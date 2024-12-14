#ifndef ORIGINAL_SPLAY_TREE_H
#define ORIGINAL_SPLAY_TREE_H

#include <bits/stdc++.h>
using namespace std;

namespace ost {

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

// Splay Tree class
struct SplayTree {
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
        Node *x = new Node();
        x->key = key;

        if (root == nullptr) {
            set_root(x);
            return;
        }

        Node *current = root;
        Node *previous = nullptr;

        while (current != nullptr) {
            previous = current;
            current = current->child[current->key < x->key];
        }

        previous->set_child(int(previous->key < x->key), x);
        splay(x);
    }

    // Find the node with the smallest key >= the given key
    Node *lower_bound(const int &key) {
        Node *current = root;
        Node *answer = nullptr;

        while (current != nullptr) {
            if (current->key < key) {
                current = current->child[1];
            } else {
                answer = current;
                current = current->child[0];
            }
        }

        splay(answer);
        return answer;
    }

    // Remove a specific node
    void remove(Node *x) {
        if (x == nullptr) return;

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
    }

    // Destructor to clear the tree when it goes out of scope
    ~SplayTree() {
        clear();
    }
};

}
#endif