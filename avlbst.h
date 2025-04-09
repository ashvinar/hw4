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
    void rotateLeft(AVLNode<Key, Value>* node);
    void rotateRight(AVLNode<Key, Value>* node);
    void insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node);
    void removeFix(AVLNode<Key, Value>* node, int diff);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    AVLNode<Key, Value>* curr = static_cast<AVLNode<Key, Value>*>(this->internalFind(new_item.first));
    AVLNode<Key, Value>* newNode = NULL;
    if(curr == NULL)
    {
        if(this->root_ == NULL)
        {
            this->root_ = new AVLNode<Key, Value>(new_item.first, new_item.second, NULL);
            return;
        }
        else
        {
            curr = static_cast<AVLNode<Key, Value>*>(this->root_);
            while(curr != NULL)
            {
                if(new_item.first < curr->getKey())
                {
                    if(curr->getLeft() == NULL)
                    {
                        newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, curr);
                        curr->setLeft(newNode);
                        curr->updateBalance(-1);
                        if (curr->getBalance() != 0)
                            insertFix(curr, newNode);
                        break;
                    }
                    else
                    {
                        curr = curr->getLeft();
                    }
                }
                else if(new_item.first > curr->getKey())
                {
                    if(curr->getRight() == NULL)
                    {
                        newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, curr);
                        curr->setRight(newNode);
                        curr->updateBalance(1);
                        if (curr->getBalance() != 0)
                            insertFix(curr, newNode);
                        break;
                    }
                    else
                    {
                        curr = curr->getRight();
                    }
                }
                else
                {
                    curr->setValue(new_item.second);
                    return;
                }
            }
        }
    }
    else
    {
        curr->setValue(new_item.second);
    }
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::remove(const Key& key)
{
    AVLNode<Key, Value>* node = static_cast<AVLNode<Key, Value>*>(this->internalFind(key));
    if (node == NULL) return;

    if (node->getLeft() != NULL && node->getRight() != NULL) {
        AVLNode<Key, Value>* pred = static_cast<AVLNode<Key, Value>*>(this->predecessor(node));
        nodeSwap(node, pred);
    }

    AVLNode<Key, Value>* parent = node->getParent();
    AVLNode<Key, Value>* child = NULL;
    bool isLeft = false;
    int diff = 0;
    if (parent != NULL && parent->getLeft() == node) {
        isLeft = true;
        diff = 1;
    }
    else if (parent != NULL) {
        diff = -1;
    }

    if (node->getLeft() != NULL) {
        child = node->getLeft();
    } else if (node->getRight() != NULL) {
        child = node->getRight();
    }

    if (child != NULL) {
        child->setParent(parent);
    }

    if (parent == NULL) {
        this->root_ = child;
        if (this->root_ != NULL) this->root_->setParent(NULL);
    } else if (isLeft) {
        parent->setLeft(child);
        removeFix(parent, diff);
    } else {
        parent->setRight(child);
        removeFix(parent, diff);
    }

    delete node;
}

template<class Key, class Value>
void AVLTree<Key, Value>::insertFix(AVLNode<Key, Value>* parent, AVLNode<Key, Value>* node)
{
    if (parent == NULL || parent->getParent() == NULL) return;

    AVLNode<Key, Value>* grand = parent->getParent();

    if (grand->getLeft() == parent)
    {
        grand->updateBalance(-1);
        if (grand->getBalance() == 0) return;
        if (grand->getBalance() == -1)
        {
            insertFix(grand, parent);
            return;
        }
        if (grand->getBalance() == -2)
        {
            if (parent->getLeft() == node)
            {
                rotateRight(grand);
                parent->setBalance(0);
                grand->setBalance(0);
            }
            else
            {
                rotateLeft(parent);
                rotateRight(grand);
                if (node->getBalance() == -1)
                {
                    parent->setBalance(0);
                    grand->setBalance(1);
                }
                else if (node->getBalance() == 0)
                {
                    parent->setBalance(0);
                    grand->setBalance(0);
                }
                else
                {
                    parent->setBalance(-1);
                    grand->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    }
    else
    {
        grand->updateBalance(1);
        if (grand->getBalance() == 0) return;
        if (grand->getBalance() == 1)
        {
            insertFix(grand, parent);
            return;
        }
        if (grand->getBalance() == 2)
        {
            if (parent->getRight() == node)
            {
                rotateLeft(grand);
                parent->setBalance(0);
                grand->setBalance(0);
            }
            else
            {
                rotateRight(parent);
                rotateLeft(grand);
                if (node->getBalance() == 1)
                {
                    parent->setBalance(0);
                    grand->setBalance(-1);
                }
                else if (node->getBalance() == 0)
                {
                    parent->setBalance(0);
                    grand->setBalance(0);
                }
                else
                {
                    parent->setBalance(1);
                    grand->setBalance(0);
                }
                node->setBalance(0);
            }
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::removeFix(AVLNode<Key, Value>* node, int diff)
{
    if (node == NULL) return;

    AVLNode<Key, Value>* parent = node->getParent();
    int nextDiff = 0;
    if (parent != NULL)
    {
        if (parent->getLeft() == node) nextDiff = 1;
        else nextDiff = -1;
    }

    node->updateBalance(diff);
    int8_t balance = node->getBalance();

    if (balance == -1 || balance == 1)
        return;

    if (balance == -2)
    {
        AVLNode<Key, Value>* left = node->getLeft();
        int8_t lbal = left->getBalance();
        if (lbal <= 0)
        {
            rotateRight(node);
            if (lbal == 0)
            {
                node->setBalance(-1);
                left->setBalance(1);
                return;
            }
            else
            {
                node->setBalance(0);
                left->setBalance(0);
                if (parent != NULL) removeFix(parent, nextDiff);
            }
        }
        else
        {
            AVLNode<Key, Value>* lr = left->getRight();
            rotateLeft(left);
            rotateRight(node);
            if (lr->getBalance() == -1)
            {
                node->setBalance(1);
                left->setBalance(0);
            }
            else if (lr->getBalance() == 0)
            {
                node->setBalance(0);
                left->setBalance(0);
            }
            else
            {
                node->setBalance(0);
                left->setBalance(-1);
            }
            lr->setBalance(0);
            if (parent != NULL) removeFix(parent, nextDiff);
        }
    }
    else if (balance == 2)
    {
        AVLNode<Key, Value>* right = node->getRight();
        int8_t rbal = right->getBalance();
        if (rbal >= 0)
        {
            rotateLeft(node);
            if (rbal == 0)
            {
                node->setBalance(1);
                right->setBalance(-1);
                return;
            }
            else
            {
                node->setBalance(0);
                right->setBalance(0);
                if (parent != NULL) removeFix(parent, nextDiff);
            }
        }
        else
        {
            AVLNode<Key, Value>* rl = right->getLeft();
            rotateRight(right);
            rotateLeft(node);
            if (rl->getBalance() == 1)
            {
                node->setBalance(-1);
                right->setBalance(0);
            }
            else if (rl->getBalance() == 0)
            {
                node->setBalance(0);
                right->setBalance(0);
            }
            else
            {
                node->setBalance(0);
                right->setBalance(1);
            }
            rl->setBalance(0);
            if (parent != NULL) removeFix(parent, nextDiff);
        }
    }
    else if (balance == 0)
    {
        if (parent != NULL) removeFix(parent, nextDiff);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateLeft(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* right = node->getRight();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setRight(right->getLeft());
    if (right->getLeft() != NULL) right->getLeft()->setParent(node);

    right->setLeft(node);
    node->setParent(right);
    right->setParent(parent);

    if (parent == NULL) {
        this->root_ = right;
    } else if (parent->getLeft() == node) {
        parent->setLeft(right);
    } else {
        parent->setRight(right);
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::rotateRight(AVLNode<Key, Value>* node)
{
    AVLNode<Key, Value>* left = node->getLeft();
    AVLNode<Key, Value>* parent = node->getParent();

    node->setLeft(left->getRight());
    if (left->getRight() != NULL) left->getRight()->setParent(node);

    left->setRight(node);
    node->setParent(left);
    left->setParent(parent);

    if (parent == NULL) {
        this->root_ = left;
    } else if (parent->getLeft() == node) {
        parent->setLeft(left);
    } else {
        parent->setRight(left);
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