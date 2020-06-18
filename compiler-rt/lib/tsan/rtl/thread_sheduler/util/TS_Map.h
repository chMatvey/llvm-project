#ifndef SANITAZER_TS_MAP_H
#define SANITAZER_TS_MAP_H


#include "TS_Iterable.h"
#include "TS_Entry.h"

namespace __tsan::ts::util {
    template <typename K, typename V>
    class TS_Map : TS_Iterable<TS_Entry<K, V>> {
    private:
        class TS_MapNode {
        public:
            K key;
            V value;
            TS_MapNode *next = nullptr;
        };

        TS_MapNode *head = nullptr;
        TS_MapNode *tail = nullptr;
        int size = 0;

    public:
        TS_Map() {}

        V search(K key) {
            TS_MapNode *node = head;
            while (node != nullptr) {
                if (node->key == key) {
                    return node->value;
                }
                node = node->next;
            }

            return nullptr;
        }

        bool isEmpty() {
            return head == nullptr;
        }

        void add(K key, V value) {
            auto *node = new TS_MapNode();
            node->key = key;
            node->value = value;

            if (isEmpty()) {
                head = node;
                tail = head;
            } else {
                tail->next = node;
            }
            tail = node;
            size++;
        }

        void remove(K key) {
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
                while (node->next != nullptr && node->key != key) {
                    node = node->next;
                }
                if (node->next != nullptr) {
                    node->next = node->next->next;
                    size--;
                }

                delete  node;
            }
        }

        TS_Iterator<TS_Entry<K, V>> iterator() override {
            auto *array = (TS_Entry<K, V> *) malloc(sizeof(TS_Entry<K, V>) * size);
            auto node = head;
            for (int i = 0; i < size; i++) {
                array[i] = TS_Entry(node->key, node->value);
                node = node->next;
            }

            return TS_Iterator<TS_Entry<K, V>>(size, array);
        }
    };
}


#endif //SANITAZER_TS_MAP_H
