#pragma once
#include <functional>
#include <queue>
#include <utility>
#include "Utils.h"

namespace Starsurge {
    template <typename T>
    class Tree {
    public:
        Tree(T t_data) {
            this->Data = t_data;
            this->parent = NULL;
        }

        Tree<T> * Subtree() {
            Tree<T> * ret = new Tree(this->Data);

            // Perform breadth-first search.
            std::queue<std::pair<Tree<T>*,Tree<T>*>> queue;
            for (size_t i = 0; i < NumChildren(); ++i) {
                queue.push(std::make_pair(Child(i), ret));
            }

            while (!queue.empty()) {
                std::pair<Tree<T>*,Tree<T>*> p = queue.front();
                queue.pop();
                Tree<T> * cur = p.first;
                Tree<T> * parent = p.second;
                Tree<T> * newCur = parent->AddChild(cur->Data);

                for (size_t i = 0; i < cur->NumChildren(); ++i) {
                    queue.push(std::make_pair(cur->Child(i), newCur));
                }
            }

            return ret;
        }

        std::vector<Tree<T>*> Search(T data) {
            std::vector<Tree<T>*> matches;
            _Search(data, matches);
            return matches;
        }
        void ForEach(std::function<void(T)> f) {
            // Perform an preorder traversal.
            f(this->Data);
            for (size_t i = 0; i < NumChildren(); ++i) {
                Child(i)->ForEach(f);
            }
        }
        unsigned int Length() {
            unsigned int len = 0;
            _Length(len);
            return len;
        }
        unsigned int Height() {
            unsigned int height = 0;
            _Height(1, height);
            return height;
        }

        void Remove(Tree<T> * node) {
            if (node == NULL)
                return;

            // Remove all the children as well.
            for (size_t i = 0; i < node->NumChildren(); ++i) {
                Remove(node->Child(i));
            }

            // If we're not root, remove node from it's parent's children list.
            if (!node->IsRoot()) {
                Tree<T> * parent = node->Parent();
                for (size_t i = 0; i < parent->NumChildren(); ++i) {
                    if (node == parent->Child(i)) {
                        parent->children.erase(parent->children.begin()+i);
                        break;
                    }
                }
            }

            // Delete node.
            delete node;
        }

        Tree<T> * AddChild(T t_data) {
            Tree<T> * child = new Tree(t_data);
            child->parent = this;
            children.push_back(child);
            return child;
        }

        void RemoveChild(size_t i) {
            if (i < 0 || i >= this->children.size())
                return;
            Remove(Child(i));
        }

        std::string ToDOT() {
            return ToDOT([](T t) { return ToString(t); });
        }

        std::string ToDOT(std::function<std::string(T)> print) {
            std::string dot = "graph tree {\n";
            Root()->_ToDOT(dot, print);
            dot += "}";
            return dot;
        }

        Tree<T> * Root() {
            Tree<T> * root = this;
            while (!root->IsRoot()) {
                root = root->Parent();
            }
            return root;
        }
        size_t NumChildren() const {
            return children.size();
        }
        bool IsRoot() const {
            return (this->parent == NULL);
        }
        bool IsLeaf() const {
            return (children.size() == 0);
        }
        Tree<T> * Child(size_t i) const {
            if (i < 0 || i >= this->children.size())
                return NULL;
            return children[i];
        }
        Tree<T> * Parent() const {
            return this->parent;
        }

        T Data;
    private:
        std::vector<Tree<T>*> children;
        Tree<T>* parent;

        void _Search(T data, std::vector<Tree<T>*> & matches) {
            // Perform a preorder traversal.
            if (this->Data == data)
                matches.push_back(this);

            for (size_t i = 0; i < NumChildren(); ++i) {
                Child(i)->_Search(data, matches);
            }
        }

        void _Length(unsigned int & len) {
            // Perform an inorder traversal.
            len += 1;
            for (size_t i = 0; i < NumChildren(); ++i) {
                Child(i)->_Length(len);
            }
        }

        void _Height(unsigned int curHeight, unsigned int & height) {
            // Perform a preorder traversal.
            if (curHeight > height)
                height = curHeight;
            for (size_t i = 0; i < NumChildren(); ++i) {
                Child(i)->_Height(curHeight+1, height);
            }
        }

        void _ToDOT(std::string & dot, std::function<std::string(T)> print) {
            dot += "\tnode"+ToString((int)this)+" [label=\""+print(this->Data)+"\"];\n";
            for (size_t i = 0; i < NumChildren(); ++i) {
                Child(i)->_ToDOT(dot, print);
                dot += "\tnode"+ToString((int)this)+" -- node"+ToString((int)Child(i))+";\n";
            }
        }
    };
}
