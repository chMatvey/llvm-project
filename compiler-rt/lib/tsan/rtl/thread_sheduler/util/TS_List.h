//
// Created by matvey on 6/6/20.
//

#ifndef SANITAZER_TS_LIST_H
#define SANITAZER_TS_LIST_H


#include "TS_Iterable.h"

namespace __tsan::ts::util {

    template <typename T>
    class TS_List : TS_Iterable<T> {
    private:
        class TS_ListNode {
        public:
            T key;
            TS_ListNode *next = nullptr;
        };

        TS_ListNode *head = nullptr;
        TS_ListNode *tail = nullptr;
        int size = 0;

    public:
        T search(T key) {
            auto node = head;
            while (node != nullptr) {
                if (node->key == key) {
                    return node->key;
                }
                node = node->next;
            }

            return nullptr;
        }

        bool isEmpty() {
            return head == nullptr;
        }

        void add(T key) {
            auto *node = new TS_ListNode();
            node->key = key;

            if (isEmpty()) {
                head = node;
                tail = head;
            } else {
                tail->next = node;
            }
            tail = node;
            size++;
        }

        void remove(T key) {
            if (isEmpty()) {
                return;
            } else if (head->key == key) {
                if (size == 1) {
                    head = nullptr;
                    tail = nullptr;
                } else {
                    head = head->next;
                }
                size--;
            } else {
                auto *node = head;
                while (node->next != nullptr && node->next->key != key) {
                    node = node->next;
                }
                if (node->next != nullptr) {
                    node->next = node->next->next;
                    size--;
                }
                delete  node;
            }
        }

        bool contains(T key) {
            return search(key) != nullptr;
        }

        T first() {
            return head;
        }

        T last() {
            return tail;
        }

        TS_Iterator<T> iterator() override {
            T array[size];
            auto node = head;
            for (int i = 0; i < size; i++) {
                array[i] = node->key;
                node = node->next;
            }

            return TS_Iterator<T>(size, array);
        }
    };
}


#endif //SANITAZER_TS_LIST_H
