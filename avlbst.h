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

template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

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
    void updateBalanceInsert(AVLNode<Key, Value>* node);
    void updateBalanceRemove(AVLNode<Key, Value>* node, int diff);
};

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
                        updateBalanceInsert(newNode);
                        break;
                    }
                    else
                    {
                        curr = curr->getLeft();
                    }
                }
                else
                {
                    if(curr->getRight() == NULL)
                    {
                        newNode = new AVLNode<Key, Value>(new_item.first, new_item.second, curr);
                        curr->setRight(newNode);
                        updateBalanceInsert(newNode);
                        break;
                    }
                    else
                    {
                        curr = curr->getRight();
                    }
                }
            }
        }
    }
    else
    {
        curr->setValue(new_item.second);
    }
}

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
    if (parent != NULL && parent->getLeft() == node) {
        isLeft = true;
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
    } else if (isLeft) {
        parent->setLeft(child);
    } else {
        parent->setRight(child);
    }

    delete node;

    if (parent != NULL) {
        if (isLeft) {
            updateBalanceRemove(parent, 1);
        } else {
            updateBalanceRemove(parent, -1);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalanceInsert(AVLNode<Key, Value>* node)
{
    if (node == nullptr) return;

    AVLNode<Key, Value>* parent = node->getParent();
    if (parent == nullptr) return;

    // Update parent's balance based on which child we inserted into
    if (node == parent->getLeft()) {
        parent->updateBalance(-1);
    } else {
        parent->updateBalance(1);
    }

    int8_t parentBalance = parent->getBalance();

    if (parentBalance == 0) {
        // Height of subtree hasn't changed, no further action needed
        return;
    }

    if (parentBalance == -1 || parentBalance == 1) {
        // Height of parent's subtree increased, recurse up
        updateBalanceInsert(parent);
    }
    else if (parentBalance == -2) {
        // Left heavy
        if (parent->getLeft() && node == parent->getLeft()->getLeft()) {
            // Zig-Zig Left Case
            rotateRight(parent);
            parent->setBalance(0);
            node->setBalance(0);
        }
        else if (parent->getLeft() && node == parent->getLeft()->getRight()) {
            // Zig-Zag Left Case
            AVLNode<Key, Value>* leftChild = parent->getLeft();
            rotateLeft(leftChild);
            rotateRight(parent);

            if (node->getBalance() == -1) {
                parent->setBalance(1);
                leftChild->setBalance(0);
            } else if (node->getBalance() == 1) {
                parent->setBalance(0);
                leftChild->setBalance(-1);
            } else {
                parent->setBalance(0);
                leftChild->setBalance(0);
            }
            node->setBalance(0);
        }
    }
    else if (parentBalance == 2) {
        // Right heavy
        if (parent->getRight() && node == parent->getRight()->getRight()) {
            // Zig-Zig Right Case
            rotateLeft(parent);
            parent->setBalance(0);
            node->setBalance(0);
        }
        else if (parent->getRight() && node == parent->getRight()->getLeft()) {
            // Zig-Zag Right Case
            AVLNode<Key, Value>* rightChild = parent->getRight();
            rotateRight(rightChild);
            rotateLeft(parent);

            if (node->getBalance() == 1) {
                parent->setBalance(-1);
                rightChild->setBalance(0);
            } else if (node->getBalance() == -1) {
                parent->setBalance(0);
                rightChild->setBalance(1);
            } else {
                parent->setBalance(0);
                rightChild->setBalance(0);
            }
            node->setBalance(0);
        }
    }
}

template<class Key, class Value>
void AVLTree<Key, Value>::updateBalanceRemove(AVLNode<Key, Value>* node, int diff)
{
    if (node == NULL) return;

    node->updateBalance(diff);
    int8_t balance = node->getBalance();

    if (balance == -1 || balance == 1) return;

    if (balance == -2) {
        AVLNode<Key, Value>* left = node->getLeft();
        int8_t lbal = left->getBalance();
        if (lbal <= 0) {
            rotateRight(node);
            if (lbal == 0) return;
        } else {
            rotateLeft(left);
            rotateRight(node);
        }
    } else if (balance == 2) {
        AVLNode<Key, Value>* right = node->getRight();
        int8_t rbal = right->getBalance();
        if (rbal >= 0) {
            rotateLeft(node);
            if (rbal == 0) return;
        } else {
            rotateRight(right);
            rotateLeft(node);
        }
    }

    AVLNode<Key, Value>* parent = node->getParent();
    if (parent != NULL) {
        if (parent->getLeft() == node) {
            updateBalanceRemove(parent, 1);
        } else {
            updateBalanceRemove(parent, -1);
        }
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

    int8_t rb= right->getBalance();

    if (rb == 0) {
        node->setBalance(1);
        right->setBalance(-1);
    } else {
        node->setBalance(0);
        right->setBalance(0);
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

    int8_t lb = left->getBalance();

    if (lb == 0) {
        node->setBalance(-1);
        left->setBalance(1);
    } else {
        node->setBalance(0);
        left->setBalance(0);
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
