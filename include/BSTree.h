#pragma once
#include <functional>
#include <stack>
#include "Utils.h"

namespace Starsurge {
    template <typename T, typename U>
    class BSTree {
    public:
        BSTree(T t_key, U t_data) {
            this->key = t_key;
            this->Data = t_data;
            this->left = NULL;
            this->right = NULL;
            this->parent = NULL;
            this->compare = [](T a,T b) { return a < b; };
        }
        BSTree(T t_key, U t_data, std::function<bool(T,T)> test) {
            this->key = t_key;
            this->Data = t_data;
            this->left = NULL;
            this->right = NULL;
            this->parent = NULL;
            this->compare = test;
        }

        BSTree<T,U> * Subtree() {
            BSTree<T,U> * ret = new BSTree<T,U>(GetKey(), this->Data, this->compare);

            std::stack<BSTree<T,U>*> s;
            BSTree<T,U> * cur = this;
            while (cur != NULL || !s.empty()) {
                while (cur != NULL) {
                    s.push(cur);
                    cur = cur->Left();
                }
                cur = s.top();
                s.pop();
                ret->Insert(cur->GetKey(), cur->Data);
                cur = cur->Right();
            }

            return ret;
        }

        BSTree<T,U> * Root() {
            BSTree<T,U> * root = this;
            while (!root->IsRoot()) {
                root = root->Parent();
            }
            return root;
        }
        BSTree<T,U> * Get(T key) {
            return Root()->_Get(key);
        }
        std::vector<BSTree<T,U>*> Search(U data) {
            std::vector<BSTree<T,U>*> matches;
            Root()->_Search(data, matches);
            return matches;
        }
        void Insert(T key, U data) {
            Root()->_Insert(key, data);
        }
        void Remove(T key) {
            Root()->_Remove(key);
        }
        void ForEach(std::function<void(T,U)> f) {
            Root()->_ForEach(f);
        }
        unsigned int Length() {
            unsigned int len = 0;
            Root()->_Length(len);
            return len;
        }
        unsigned int Height() {
            unsigned int height = 0;
            Root()->_Height(1, height);
            return height;
        }
        BSTree<T,U> * FindMin() {
            BSTree<T,U> * result = this;
            while (result->HasLeftChild())
                result = result->Left();
            return result;
        }
        BSTree<T,U> * FindMax() {
            BSTree<T,U> * result = this;
            while (result->HasRightChild())
                result = result->Right();
            return result;
        }

        std::string ToDOT() {
            return ToDOT([](T t) { return ToString(t); });
        }

        std::string ToDOT(std::function<std::string(T)> print) {
            std::string dot = "graph bstree {\n";
            Root()->_ToDOT(dot, print);
            dot += "}";
            return dot;
        }

        bool HasLeftChild() const {
            return (this->left != NULL);
        }
        bool HasRightChild() const {
            return (this->right != NULL);
        }
        bool IsLeftChild() const {
            return (this->parent && this->parent->left == this);
        }
        bool IsRightChild() const {
            return (this->parent && this->parent->right == this);
        }
        bool IsRoot() const {
            return (this->parent == NULL);
        }
        bool IsLeaf() const {
            return (this->left == NULL && this->right == NULL);
        }
        T GetKey() const {
            return this->key;
        }
        BSTree<T,U> * Left() const {
            return this->left;
        }
        BSTree<T,U> * Right() const {
            return this->right;
        }
        BSTree<T,U> * Parent() const {
            return this->parent;
        }

        U Data;
    private:
        T key;
        BSTree<T,U> * left;
        BSTree<T,U> * right;
        BSTree<T,U> * parent;
        std::function<bool(T,T)> compare;

        BSTree<T,U> * _Get(T key) {
            if (key == GetKey()) {
                return this;
            }
            else if (this->compare(key, GetKey())) {
                if (!HasLeftChild()) {
                    return NULL;
                }
                return Left()->_Get(key);
            }
            else {
                if (!HasRightChild()) {
                    return NULL;
                }
                return Right()->_Get(key);
            }
        }

        void _Search(U data, std::vector<BSTree<T,U>*> & matches) {
            // Perform a preorder traversal.
            if (this->Data == data)
                matches.push_back(this);

            if (HasLeftChild()) {
                Left()->_Search(data, matches);
            }
            if (HasRightChild()) {
                Right()->_Search(data, matches);
            }
        }

        void _Insert(T key, U data) {
            if (key == GetKey()) {
                this->Data = data;
                return;
            }
            else if (this->compare(key, GetKey())) {
                if (!HasLeftChild()) {
                    this->left = new BSTree(key, data, this->compare);
                    this->left->parent = this;
                    return;
                }
                Left()->_Insert(key, data);
            }
            else {
                if (!HasRightChild()) {
                    this->right = new BSTree(key, data, this->compare);
                    this->right->parent = this;
                    return;
                }
                Right()->_Insert(key, data);
            }
        }

        void _Remove(T key) {
            if (key == GetKey()) {
                if (HasLeftChild() && HasRightChild()) {
                    // Copy the left-most child on the right.
                    BSTree<T,U> * successor = Right();
                    while (successor->HasLeftChild()) {
                        successor = successor->Left();
                    }
                    this->key = successor->GetKey();
                    this->Data = successor->Data;
                    successor->_Remove(successor->GetKey());
                    return;
                }
                else if (!IsLeaf()) {
                    // Set the parent's left/right to this left/right.
                    BSTree<T,U> * target = Left();
                    if (!HasLeftChild())
                        target = Right();

                    if (!IsRoot()) {
                        if (IsLeftChild())
                            Parent()->left = target;
                        else
                            Parent()->right = target;
                    }
                    target->parent = Parent();
                }
                else {
                    // Set the parent's left/right to NULL.
                    if (!IsRoot()) {
                        if (IsLeftChild())
                            Parent()->left = NULL;
                        else
                            Parent()->right = NULL;
                    }
                }
                delete this; // Commit suicide.
                return;
            }
            else if (this->compare(key, GetKey())) {
                if (Left() == NULL) {
                    return;
                }
                Left()->_Remove(key);
            }
            else {
                if (Right() == NULL) {
                    return;
                }
                Right()->_Remove(key);
            }
        }

        void _ForEach(std::function<void(T,U)> f) {
            // Perform an inorder traversal.
            if (HasLeftChild())
                Left()->_ForEach(f);
            f(GetKey(), this->Data);
            if (HasRightChild())
                Right()->_ForEach(f);
        }

        void _Length(unsigned int & len) {
            // Perform an inorder traversal.
            if (HasLeftChild())
                Left()->_Length(len);
            len += 1;
            if (HasRightChild())
                Right()->_Length(len);
        }

        void _Height(unsigned int curHeight, unsigned int & height) {
            // Perform a preorder traversal.
            if (curHeight > height)
                height = curHeight;
            if (HasLeftChild())
                Left()->_Height(curHeight+1, height);
            if (HasRightChild())
                Right()->_Height(curHeight+1, height);
        }

        void _ToDOT(std::string & dot, std::function<std::string(T)> print) {
            dot += "\tnode"+ToString((int)this)+" [label=\""+print(this->key)+"\"];\n";
            if (Left()) {
                Left()->_ToDOT(dot, print);
                dot += "\tnode"+ToString((int)this)+" -- node"+ToString((int)this->left)+";\n";
            }
            if (Right()) {
                Right()->_ToDOT(dot, print);
                dot += "\tnode"+ToString((int)this)+" -- node"+ToString((int)this->right)+";\n";
            }
        }
    };

    /*template<typename T>
    class BSTree {
    public:
        BSTree() {
            this->root = NULL;
            this->compare = [](T a,T b) { return a < b; };
        }
        BSTree(T data) {
            this->root = new BSTreeNode(data);
            this->compare = [](T a,T b) { return a < b; };
        }
        BSTree(T data, std::function<bool(T,T)> test) {
            this->root = new BSTreeNode(data);
            this->compare = test;
        }
        BSTree(BSTreeNode<T> * root) {
            this->root = root;
            this->compare = [](T a,T b) { return a < b; };
        }
        BSTree(BSTreeNode<T> * root, std::function<bool(T,T)> test) {
            this->root = root;
            this->compare = test;
        }
        BSTree(const BSTree<T>& copy) {
            this->compare = copy.compare;
            Insert(copy.GetRoot());
        }

        BSTreeNode<T> * Search(T data) {
            return NULL;
        }
        void Insert(T data) {
            BSTreeNode<T> ** walk = &this->root;
            while ((*walk) != NULL) {
                T walk_data = (*walk)->data;
                if (walk_data == data) {
                    return;
                }
                if (compare(data, walk_data)) {
                    walk = &(*walk)->left;
                }
                else {
                    walk = &(*walk)->right;
                }
            }
            *walk = new BSTreeNode<T>(data);
        }
        void Insert(BSTreeNode<T> * subtree) {
            if (subtree == NULL) {
                return;
            }

        }
        void Remove(T data) {
            BSTreeNode<T> ** parent = NULL;
            BSTreeNode<T> * walk = &this->root;
            while (walk != NULL) {
                T walk_data = walk->data;
                if (walk_data == data) {
                    if (walk->left != NULL && walk->right != NULL) {
                        // Find the left-most child of walk->right.
                        BSTreeNode<T> * successor = walk->right;
                        while (successor->left != NULL) {
                            successor = successor->left;
                        }
                        T data = successor->data;
                        delete successor;
                        return;
                    }
                    else if (walk->left != NULL) {
                        *parent = walk->left;
                    }
                    else if (walk->right != NULL) {
                        *parent = walk->right;
                    }
                    delete walk;
                    return;
                }
                if (compare(data, walk_data)) {
                    parent = &walk->left;
                    walk = walk->left;
                }
                else {
                    parent = &walk->right;
                    walk = walk->right;
                }
            }
        }
        void ForEach(std::function<void(T)> callback) {

        }

        BSTreeNode<T> * GetRoot() {
            return this->root;
        }
        std::string ToDOT() const {
            std::string dot = "digraph bstree {\n";
            if (this->root)
                this->root->_ToDOT(dot);
            dot += "}";
            return dot;
        }

    private:
        BSTreeNode<T> * root;
        std::function<bool(T,T)> compare;
    };*/
}
