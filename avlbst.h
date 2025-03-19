#ifndef AVLBST_H
#define AVLBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove(const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    void rotateLeft(AVLNode<Key, Value>* node);  //insert helper
    void rotateRight(AVLNode<Key, Value>* node); //insert helper
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);   //insert helper 

    void removeFix(AVLNode<Key, Value>* node, int diff); //remove helper
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert(const std::pair<const Key, Value> &new_item)
{
    // TODO
    AVLNode<Key, Value>* newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, nullptr);
    if(this->root_ == nullptr)
    {
        this->root_ = newNode;
        return;
    }

    AVLNode<Key, Value>* current = static_cast<AVLNode<Key, Value>*>(this->root_);
    AVLNode<Key, Value>* parent = nullptr;

    //find the correct spot to insert the new node - at leaf
    while(current != nullptr)
    {
        parent = current;
        //if new key is less than current key, go left
        if(new_item.first < current->getKey())
        {
            current = current->getLeft();
        }
        //if new key is greater than current key, go right
        else if(new_item.first > current->getKey())
        {
            current = current->getRight();
        }
        //if new key is equal to current, update
        else
        {
            current->setValue(new_item.second);
            delete newNode;
            return;
        }
    }

    //insert new node
    newNode->setParent(parent);
    //if new key is less than parent key, insert left
    if(new_item.first < parent->getKey())
    {
        parent->setLeft(newNode);
    }
    //if new key is greater than parent key, insert right
    else
    {
        parent->setRight(newNode);
    }

    //update balance factors
    newNode->setBalance(0);

    //fix balance factors
    if(parent->getBalance() == -1)
    {
        parent->setBalance(0);
    }
    else if(parent->getBalance() == 1)
    {
        parent->setBalance(0);
    }
    else if(parent->getBalance() == 0)
    {
        if(newNode == parent->getLeft())
        {
            parent->setBalance(-1);
        }
        else
        {
            parent->setBalance(1);
        }

        insertFix(parent, newNode);

    }
}



/*
 * helper function for insert - rotate left
 * Left Rotation is taking a right child, making it the parent and making the original parent the new left child 
 * balances right subtree when its too tall 
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    //get right child - will become new root
    AVLNode<Key, Value>* rightChild = node->getRight();

    //sets right childs left subtree to node's right subtree
    node->setRight(rightChild->getLeft());

    //update parent pointer of left sub of right child, if not null. Node is its parent now
    if(rightChild->getLeft() != nullptr)
    {
        rightChild->getLeft()->setParent(node);
    }

    //set right child as the parent of node. Right child takes node place
    rightChild->setParent(node->getParent());

    //update children - connects right child to nodes original parent before rotation (since its been replaced)
    //case 1 - node was the root 
    if(node->getParent() == nullptr)
    {
        this->root_ = rightChild;
    }
    //case 2 - node was a left child
    else if(node == node->getParent()->getLeft())
    {
        node->getParent()->setLeft(rightChild);
    }
    //case 3 - node was a right child
    else
    {
        node->getParent()->setRight(rightChild);
    }

    //complete rotation - node is now the left child of right child 
    rightChild->setLeft(node);
    node->setParent(rightChild);

    //update balance factors - height of nodes must be updated after rotation
    //nodes new balance factor is updated by its original balance and the max height balance of right childs subtrees
    node->setBalance(node->getBalance() - 1 - std::max(0, rightChild->getBalance()));

    //rightchilds new balance factor is updated by its original balance and the min height balance of nodes subtrees
    rightChild->setBalance(rightChild->getBalance() - 1 + std::min(0, node->getBalance()));

}

/* 
 * helper function for insert - rotate right
 * Right Rotation is taking a left child, making it the parent and making the original parent the new right child
 */
template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    //get left child - will become new root
    AVLNode<Key, Value>* leftChild = node->getLeft();

    //sets left childs right subtree to nodes left subtree
    node->setLeft(leftChild->getRight());

    //update parent pointer of right sub of left child, if not null. Node is its parent now
    if(leftChild->getRight() != nullptr)
    {
        leftChild->getRight()->setParent(node);
    }

    //set left child as the parent of node. Left child takes node place
    leftChild->setParent(node->getParent());

    //update children - connects left child to nodes original parent before rotation (since its been replaced)
    //case 1 - node was the root
    if(node->getParent() == nullptr)
    {
        this->root_ = leftChild;
    }
    //case 2 - node was a left child
    else if(node == node->getParent()->getLeft())
    {
        node->getParent()->setLeft(leftChild);
    }
    //case 3 - node was a right child
    else
    {
        node->getParent()->setRight(leftChild);
    }

    //complete rotation - node is now the right child of left child
    leftChild->setRight(node);
    node->setParent(leftChild);

    //update balance factors - height of nodes must be updated after rotation
    //nodes new balance factor is updated by its original balance and the min height balance of left childs subtrees
    node->setBalance(node->getBalance() + 1 - std::min(0, leftChild->getBalance()));

    //leftchilds new balance factor is updated by its original balance and the max height balance of nodes subtrees
    leftChild->setBalance(leftChild->getBalance() + 1 + std::max(0, node->getBalance()));

}


//helper function for insert - fix the balance after rotations
template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node)
{
    //if p is null, return
    if(parent == nullptr || parent->getParent() == nullptr)
    {
        return;
    }

    AVLNode<Key, Value>* grandparent = parent->getParent();


    //Case 1 - parent is left child of grandparent (assume)
    if(grandparent->getLeft() == parent) 
    {
        //left sub tree is taller, decrement balance
        grandparent->updateBalance(-1);

        //subcase 1 - balance is 0
        if(grandparent->getBalance() == 0) 
        {
            //tree is balanced
            return;
        } 
        //subcase 2 - balance is -1
        else if (grandparent->getBalance() == -1) 
        {
            //recurse up tree
            insertFix(grandparent, parent);
        } 
        //subcase 3 - balance is -2
        else if(grandparent->getBalance() == -2)
        {
            //rotations

            //Zig-Zig (left-left)
            if(parent->getLeft() == node) 
            {
                rotateRight(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            } 
            //Zig-Zag (left-right)
            else 
            {
                rotateLeft(parent);
                rotateRight(grandparent);

                //update balance factors
                //subcase 1 - balance is -1. parent = 0, gp = 1, node = 0
                if(node->getBalance() == -1) 
                {
                    parent->setBalance(0);
                    grandparent->setBalance(1);
                    node->setBalance(0);
                } 
                //subcase 2 - balance is 0. parent = 0, gp = 0, node = 0
                else if(node->getBalance() == 0) 
                {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                } 
                //subcase 3 - balance is 1. parent = -1, gp = 0, node = 0
                else if(node->getBalance() == 1)
                {
                    parent->setBalance(-1);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    } 

    //Case 2 - parent is right child of grandparent 
    else //grandparent->getRight() == parent
    {
        //right subtre taller, increment balance factor
        grandparent->updateBalance(1);

        //subcase 1 - balance is 0
        if (grandparent->getBalance() == 0) 
        {
            //tree is balanced
            return;
        } 
        //subcase 2 - balance is 1
        else if(grandparent->getBalance() == 1) 
        {
            //recurse up tree
            insertFix(grandparent, parent);
        } 
        //subcase 3 - balance is 2
        else if(grandparent->getBalance() == 2)
        {
            //rotations

            //Zig-Zig (right-right)
            if (parent->getRight() == node) 
            {
                rotateLeft(grandparent);
                parent->setBalance(0);
                grandparent->setBalance(0);
            } 
            //Zig-Zag (right-left)
            else 
            {
                rotateRight(parent);
                rotateLeft(grandparent);

                //update balance factors
                //subcase 1 - balance is 1. parent = 0, gp = -1, node = 0
                if(node->getBalance() == 1) 
                {
                    parent->setBalance(0);
                    grandparent->setBalance(-1);
                    node->setBalance(0);
                } 
                //subcase 2 - balance is 0. parent = 0, gp = 0, node = 0
                else if(node->getBalance() == 0) 
                {
                    parent->setBalance(0);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                } 
                //subcase 3 - balance is -1. parent = 1, gp = 0, node = 0
                else if(node->getBalance() == -1)
                {
                    parent->setBalance(1);
                    grandparent->setBalance(0);
                    node->setBalance(0);
                }
            }
        }
    }
}






/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    // TODO
    AVLNode<Key, Value>* removeNode = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));

    //if key is not in tree
    if(removeNode == nullptr)
    {
        return;
    }

    //case 1 - node has 2 children - swap with predecessor
    if(removeNode->getLeft() != nullptr && removeNode->getRight() != nullptr)
    {
        AVLNode<Key, Value>* pred = dynamic_cast<AVLNode<Key, Value>*>(this->predecessor(dynamic_cast<Node<Key, Value>*>(removeNode)));
        nodeSwap(removeNode, pred);
    }

    //case 2 - node has 1 or 0 children
    AVLNode<Key, Value>* parent = removeNode->getParent();
    AVLNode<Key, Value>* child = (removeNode->getLeft() != nullptr) ? removeNode->getLeft() : removeNode->getRight();
    int diff = 0;

    //set diff based on child that is being removed
    if(parent != nullptr)
    {
        //if removeNode has a left child
        if(removeNode == parent->getLeft())
        {
            diff = 1;
        }
        //if removeNode has a right child
        else
        {
            diff = -1;
        }
    }

    //if removeNode is the root
    if(parent == nullptr)
    {
        this->root_ = child;
    }
    //if removeNode is a left child
    else if(removeNode == parent->getLeft())
    {
        parent->setLeft(child);
    }
    //if removeNode is a right child
    else
    {
        parent->setRight(child);
    }

    if(child != nullptr)
    {
        child->setParent(parent);
    }

    //delete node
    delete removeNode;

    if(parent != nullptr)
    {
        removeFix(parent, diff);
    }

}


/*
* helper function for remove - fix the balance after rotations
* ndiff is the difference in height of the node's subtree after the node is removed
*/
template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
    //if p is null
    if(node == nullptr)
    {
        return;
    }

    //Case for diff = -1  (left sub taller after removal) (assume diff = -1 then mirror after)
    if(diff == -1)
    {
        int newBalance = node->getBalance() + diff;

        //Case 1 - balance becomes -2
        if(newBalance == -2)
        {
            AVLNode<Key, Value>* c = node->getLeft();

            //subcase 1a - Zig-Zig (left-left)
            if(c != nullptr && c->getBalance() == -1)
            {
                rotateRight(node);
                node->setBalance(0);
                c->setBalance(0);
                //ndiff is +1 if n is a left child and -1 otherwise
                removeFix(node->getParent(), (node->getParent() && node == node->getParent()->getLeft()) ? 1 : -1);
            }

            //subcase 1b - Zig-Zig with balance 0
            else if(c != nullptr && c->getBalance() == 0)
            {
                rotateRight(node);
                node->setBalance(-1);
                c->setBalance(1);
            }

            //subcase 1c - Zig-Zag (left-right)
            else if(c != nullptr && c->getBalance() == 1)
            {
                AVLNode<Key, Value>* g = c->getRight();

                rotateLeft(c);
                rotateRight(node);

                //update balance factors
                //1 - if g = 1 then n = 0, c = -1, g = 0
                if(g->getBalance() == 1)
                {
                    node->setBalance(0);
                    c->setBalance(-1);
                    g->setBalance(0);
                }

                //2 - if g = 0 then n = 0, c = 0, g = 0
                else if(g->getBalance() == 0)
                {
                    node->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }

                //3 - if g = -1 then n = 1, c = 0, g = 0
                else if(g->getBalance() == -1)
                {
                    node->setBalance(1);
                    c->setBalance(0);
                    g->setBalance(0);
                }

                //ndiff is +1 if n is a left child and -1 otherwise
                removeFix(node->getParent(), (node->getParent() && node == node->getParent()->getLeft()) ? 1 : -1);
            }
        }

        //Case 2 - balance becomes -1
        else if(newBalance == -1)
        {
            node->setBalance(-1);
        }

        //Case 3 - balance becomes 0
        else if(newBalance == 0)
        {
            node->setBalance(0);
            //ndiff is +1 if n is a left child and -1 otherwise
            removeFix(node->getParent(), (node->getParent() && node == node->getParent()->getLeft()) ? 1 : -1);
        }
    }

    //Case for diff = 1 (right sub taller after removal) MIRROR CASE
    else if(diff == 1)
    {
        int newBalance = node->getBalance() + diff;

        //Case 1 - balance becomes 2
        if(newBalance == 2)
        {
            AVLNode<Key, Value>* c = node->getRight();

            //subcase 1a - Zig-Zig (right-right)
            if(c != nullptr && c->getBalance() == 1)
            {
                rotateLeft(node);
                node->setBalance(0);
                c->setBalance(0);
                //ndiff is +1 if n is a left child and -1 otherwise
                removeFix(node->getParent(), (node->getParent() && node == node->getParent()->getLeft()) ? 1 : -1);            
            }

            //subcase 1b - Zig-Zig with balance 0
            else if(c != nullptr && c->getBalance() == 0)
            {
                rotateLeft(node);
                node->setBalance(1);
                c->setBalance(-1);
            }

            //subcase 1c - Zig-Zag (right-left)
            else if(c != nullptr && c->getBalance() == -1)
            {
                AVLNode<Key, Value>* g = c->getLeft();

                rotateRight(c);
                rotateLeft(node);

                //update balance factors
                //1 - if g = -1 then n = 0, c = 1, g = 0
                if(g->getBalance() == -1)
                {
                    node->setBalance(0);
                    c->setBalance(1);
                    g->setBalance(0);
                }

                //2 - if g = 0 then n = 0, c = 0, g = 0
                else if(g->getBalance() == 0)
                {
                    node->setBalance(0);
                    c->setBalance(0);
                    g->setBalance(0);
                }

                //3 - if g = 1 then n = -1, c = 0, g = 0
                else if(g->getBalance() == 1)
                {
                    node->setBalance(-1);
                    c->setBalance(0);
                    g->setBalance(0);
                }
                //ndiff is +1 if n is a left child and -1 otherwise
                removeFix(node->getParent(), (node->getParent() && node == node->getParent()->getLeft()) ? 1 : -1);            
            }
        }

        //Case 2 - balance becomes 1
        else if(newBalance == 1)
        {
            node->setBalance(1);
        }

        //Case 3 - balance becomes 0
        else if(newBalance == 0)
        {
            node->setBalance(0);
            //ndiff is +1 if n is a left child and -1 otherwise
            removeFix(node->getParent(), (node->getParent() && node == node->getParent()->getLeft()) ? 1 : -1);        
        }
    }

}

template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
