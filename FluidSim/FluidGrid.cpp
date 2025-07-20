#include "FluidGrid.h"
#include <algorithm>
#include <cassert>

// Helper for comparing GridKey tuples
static bool keyLess(const std::tuple<int, int, int>& a, const std::tuple<int, int, int>& b) {
	return a < b; // Compares tuples lexicographically (x=x, then  y=y, then z=z)
}

FluidGrid::FluidGrid(int order)
    : order(order), root(std::make_shared<Node>(true)), width(0), height(0), depth(0) {}

FluidGrid::Cell* FluidGrid::find(int x, int y, int z) {
    std::tuple<int, int, int> key(x, y, z);
    return findInNode(root, key);
}

FluidGrid::Cell* FluidGrid::findInNode(std::shared_ptr<Node> node, const std::tuple<int, int, int>& key) {
    auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key, keyLess);
    int idx = static_cast<int>(it - node->keys.begin());
    if (node->isLeaf) {
        if (it != node->keys.end() && *it == key)
            return &node->values[idx];
        return nullptr;
    } else {
        if (idx < static_cast<int>(node->keys.size()) && key == node->keys[idx])
            ++idx;
        return findInNode(node->children[idx], key);
    }
}

void FluidGrid::insert(int x, int y, int z, const Cell& cell) {
    std::tuple<int, int, int> key(x, y, z);
    if (root->keys.size() == 2 * order - 1) {
        auto s = std::make_shared<Node>(false);
        s->children.push_back(root);
        splitChild(s, 0);
        root = s;
    }
    insertNonFull(root, key, cell);
}

void FluidGrid::insertNonFull(std::shared_ptr<Node> node, const std::tuple<int, int, int>& key, const Cell& cell) {
    if (node->isLeaf) {
        auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key, keyLess);
        int idx = static_cast<int>(it - node->keys.begin());
        if (it != node->keys.end() && *it == key) {
            node->values[idx] = cell; // update
        } else {
            node->keys.insert(it, key);
            node->values.insert(node->values.begin() + idx, cell);
        }
    } else {
        auto it = std::lower_bound(node->keys.begin(), node->keys.end(), key, keyLess);
        int idx = static_cast<int>(it - node->keys.begin());
        if (node->children[idx]->keys.size() == 2 * order - 1) {
            splitChild(node, idx);
            if (keyLess(node->keys[idx], key))
                ++idx;
        }
        insertNonFull(node->children[idx], key, cell);
    }
}

void FluidGrid::splitChild(std::shared_ptr<Node> parent, int idx) {
    auto y = parent->children[idx];
    auto z = std::make_shared<Node>(y->isLeaf);
    int t = order;
    parent->keys.insert(parent->keys.begin() + idx, y->keys[t - 1]);
    parent->children.insert(parent->children.begin() + idx + 1, z);

    z->keys.assign(y->keys.begin() + t, y->keys.end());
    y->keys.resize(t - 1);

    if (y->isLeaf) {
        z->values.assign(y->values.begin() + t, y->values.end());
        y->values.resize(t - 1);
        z->next = y->next;
        y->next = z;
    } else {
        z->children.assign(y->children.begin() + t, y->children.end());
        y->children.resize(t);
    }
}