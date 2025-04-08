#ifndef RECCHECK
// Add any #includes here if needed
#include <utility>
#include <algorithm>
#endif//check this

#include "equal-paths.h"
using namespace std;

// You may add any prototypes of helper functions here
std::pair<int, int> leafDepths(Node* node);

bool equalPaths(Node* root)
{
    if (root == nullptr) return true;

    std::pair<int, int> depths = leafDepths(root);
    return depths.first == depths.second;
}

std::pair<int, int> leafDepths(Node* node)
{
    // Base case: leaf node
    if (node->left == nullptr && node->right == nullptr)
    {
        return {0, 0}; // leaf is at depth 0 from itself
    }

    // Only left child exists
    if (node->left != nullptr && node->right == nullptr)
    {
        std::pair<int, int> leftPair = leafDepths(node->left);
        return {leftPair.first + 1, leftPair.second + 1};
    }

    // Only right child exists
    if (node->left == nullptr && node->right != nullptr)
    {
        std::pair<int, int> rightPair = leafDepths(node->right);
        return {rightPair.first + 1, rightPair.second + 1};
    }

    // Both children exist
    std::pair<int, int> leftPair = leafDepths(node->left);
    std::pair<int, int> rightPair = leafDepths(node->right);

    int minDepth = std::min(leftPair.first, rightPair.first) + 1;
    int maxDepth = std::max(leftPair.second, rightPair.second) + 1;

    return {minDepth, maxDepth};
}
