#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)
#include <iostream>
#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


bool equalPathsCheck(Node* root, int depth, int& leafDepth) 
{
    //base case - empty tree 
    if (root == nullptr) 
    {
        return true;
    }

    //base case - leaf node
    if(root->left == nullptr && root->right == nullptr) 
    {
        //first leaf node not found yet (FIRST LEAF NODE ONLY)
        if(leafDepth == -1) 
        {
            //first leaf node found, set its depth to depth
            leafDepth = depth;
        }
        //compare the depth of this leaf with the first leaf's depth
        return depth == leafDepth;
    }

    //check left and right subtrees rescursively
    return equalPathsCheck(root->left, depth + 1, leafDepth) && equalPathsCheck(root->right, depth + 1, leafDepth);
}


bool equalPaths(Node* root) 
{
    //a leaf node has not been found yet
    int leafDepth = -1; 

    return equalPathsCheck(root, 0, leafDepth);
}

