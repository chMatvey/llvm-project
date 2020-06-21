#ifndef SANITAZER_TS_MAP_H
#define SANITAZER_TS_MAP_H


#include "TS_Iterable.h"
#include "TS_Entry.h"
#include "TS_Comparator.h"

#include <sanitizer_common/sanitizer_allocator_internal.h>

namespace __tsan::ts::util {
    template<typename K, typename V>
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
        TS_Comparator<K> *comparator = nullptr;

    public:
        TS_Map() {}

        TS_Map(TS_Comparator<K> *comparator) : comparator(comparator) {}

        V search(K key) {
            TS_MapNode *node = head;
            while (node != nullptr) {
                if (node->key == key || (comparator != nullptr && comparator->compare(node->key, key) == 0)) {
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
            } else if (head->key == key || (comparator != nullptr && comparator->compare(head->key, key) == 0)) {
                if (size == 1) {
                    head = nullptr;
                    tail = nullptr;
                } else {
                    head = head->next;
                }
                size--;
            } else {
                auto *node = head;
                if (comparator == nullptr) {
                    while (node->next != nullptr && node->next->key != key) {
                        node = node->next;
                    }
                } else {
                    while (node->next != nullptr && comparator->compare(node->next->key, key) != 0) {
                        node = node->next;
                    }
                }
                if (node->next != nullptr) {
                    auto *removed = node->next;
                    node->next = node->next->next;
                    size--;
                    delete removed;
                }
            }
        }

        void setComparator(TS_Comparator<K> *tsComparator) {
            comparator = tsComparator;
        }

        TS_Iterator<TS_Entry<K, V>> iterator() override {
            auto *array = static_cast<TS_Entry<K, V> *>(InternalCalloc(size, sizeof(TS_Entry<K, V>)));
            auto node = head;
            for (int i = 0; i < size; i++) {
                array[i] = TS_Entry<K, V>(node->key, node->value);
                node = node->next;
            }

            return TS_Iterator<TS_Entry<K, V>>(size, array);
        }
    };
}


#endif //SANITAZER_TS_MAP_H
