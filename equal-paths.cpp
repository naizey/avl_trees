#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here


int countPathSteps(Node * root)
{
    //base case, empty tree
    if(root == nullptr)
    {
        return 0;
    }

    //otherwise, recursively get paths of left and right subtrees
    int countLeft = countPathSteps(root->left);
    int countRight = countPathSteps(root->right);

    //return whichever is the longest path of the two including the current node (path from current node to leaf node)
    return max(countLeft, countRight) + 1;

}

bool equalPaths(Node * root)
{
    // Add your code below
    if (root == nullptr)
    {
        return true;
    }
    
    //get the path lengths of the left and right subtrees
    int leftPath = countPathSteps(root->left);
    int rightPath = countPathSteps(root->right);

    if((leftPath != rightPath) && (leftPath != 0 && rightPath != 0))
    {
        return false;
    }

    //recursively check the left and right subtrees
    return equalPaths(root->left) && equalPaths(root->right);
    
}





// bool checkEqualPaths(Node* root, int depth, int& leafDepth) 
// {
//     //base case - tree 
//     if (root == nullptr) 
//     {
//         return true;
//     }

//     // If it's a leaf node
//     if (root->left == nullptr && root->right == nullptr) 
//     {
//         if (leafDepth == -1) 
//         {
//             // First leaf node found, set the leafDepth
//             leafDepth = depth;
//         }
//         // Compare the depth of this leaf with the first leaf's depth
//         return depth == leafDepth;
//     }

//     // Recursively check left and right subtrees
//     return checkEqualPaths(root->left, depth + 1, leafDepth) && checkEqualPaths(root->right, depth + 1, leafDepth);
// }

// bool equalPaths(Node* root) 
// {
//     int leafDepth = -1; // Initialize leafDepth to an invalid value
//     return checkEqualPaths(root, 0, leafDepth);
// }

