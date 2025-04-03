#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here

std::pair<int, int> leafDepths(Node* node, int currentDepth);

bool equalPaths(Node * root)
{
    // Add your code below

    if (root == nullptr) return true;

}


std::pair<int, int> leafDepths(Node* node, int currentDepth){

  if (node->left == nullptr && node->right == nullptr){
    return {currentDepth, currentDepth};
  }

  if (node->left != nullptr)
    std::pair<int,int>leftPair = computeLeafDepths(node->left, currentDepth + 1);
  
  if (node->right != nullptr)
    std::pair<int,int>rightPair = computeLeafDepths(node->right, currentDepth + 1);

  if (node->right != nullptr && node->left != nullptr)
    std::pair<int,int>leftPair = computeLeafDepths(node->left, currentDepth + 1);
    std::pair<int,int>rightPair = computeLeafDepths(node->right, currentDepth + 1);

    minDepth = min(leftPair)

}
