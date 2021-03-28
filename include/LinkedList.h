#pragma once

namespace Starsurge {
    template <typename T>
    struct LinkedList {
        LinkedList(T data) {
            this->Data = data;
            this->next = NULL;
            this->prev = NULL;
        }
        LinkedList(T data, LinkedList * t_prev, LinkedList * t_next) {
            this->Data = data;
            this->next = t_next;
            this->prev = t_prev;
        }
        void AddNode(T data) {
            AddNode(new LinkedList<T>(data));
        }
        void AddNode(LinkedList<T> * node) {
            LinkedList<T> * head = Head();
            if (head != NULL) {
                Head()->InsertNode(node);
            }
        }
        void InsertNode(T data) {
            InsertNode(new LinkedList<T>(data));
        }
        void InsertNode(LinkedList<T> * node) {
            if (node == NULL) {
                return;
            }
            else {
                LinkedList<T> * thisnext = this->Next();
                this->next = node;

                LinkedList<T> * head = node->Head();
                head->next = thisnext;
                if (thisnext != NULL) {
                    thisnext->prev = head;
                }
            }
        }
        void RemoveNode(LinkedList<T> * node) {
            if (node == NULL) {
                return;
            }
            LinkedList<T> * nodeprev = node->Prev();
            LinkedList<T> * nodenext = node->Next();
            if (nodeprev != NULL) {
                nodeprev->next = nodenext;
            }
            if (nodenext != NULL) {
                nodenext->prev = nodeprev;
            }
            delete node;
        }
        LinkedList * Next() {
            return this->next;
        }
        LinkedList * Prev() {
            return this->prev;
        }
        LinkedList * Head() {
            LinkedList<T> * node = this;
            while (node->Next() != NULL) {
                node = node->Next();
            }
            return node;
        }
        LinkedList * Tail() {
            LinkedList * node = this;
            while (node->Prev() != NULL) {
                node = node->Prev();
            }
            return node;
        }
        unsigned int Size() {
            unsigned int count = 1;
            LinkedList<T> * node = Tail();
            if (node == NULL) {
                return 0;
            }
            while (node->Next() != NULL) {
                node = node->Next();
                count += 1;
            }
            return count;
        }

        T Data;
    private:
        LinkedList * next, * prev;
    };
}
