#ifndef SANITIZER_COMMON_CUSTOM_DEADLOCK_DETECTOR_H
#define SANITIZER_COMMON_CUSTOM_DEADLOCK_DETECTOR_H

#pragma once

template <typename T>
class custom_iterator {
private:
    int size;
    int current_index = 0;
    T *array;

public:
    custom_iterator(int size, T *array) : size(size), array(array) {}

    bool has_next() {
        return current_index != size;
    }

    T next() {
        T result = array[current_index];
        current_index++;

        return result;
    }
};

template <typename T>
class custom_iterable {
public:
    virtual custom_iterator<T> get_iterator() {}
};

template <typename T>
class custom_list : custom_iterable<T> {
private:
    class list_node {
    public:
        T key;
        list_node *next = nullptr;
    };

    list_node *head = nullptr;
    list_node *tail = nullptr;
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

    bool is_empty() {
        return head == nullptr;
    }

    void add(T key) {
        auto *node = new list_node();
        node->key = key;

        if (is_empty()) {
            head = node;
        } else {
            tail->next = node;
        }
        tail = node;
        size++;
    }

    void remove(T key) {
        if (is_empty()) {
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

    T first() {
        return head;
    }

    T last() {
        return tail;
    }

    custom_iterator<T> get_iterator() override {
        T array[size];
        auto node = head;
        for (int i = 0; i < size; i++) {
            array[i] = node->key;
            node = node->next;
        }

        return custom_iterator<T>(size, array);
    }
};

template <typename K, typename V>
class custom_map : custom_iterable<V> {
private:
    class map_node {
    public:
        K key;
        V value;
        map_node *next = nullptr;
    };

    map_node *head = nullptr;
    map_node *tail = nullptr;
    int size = 0;

public:
    V search(K key) {
        map_node *node = head;
        while (node != nullptr) {
            if (node->key == key) {
                return node->value;
            }
            node = node->next;
        }

        return nullptr;
    }

    bool is_empty() {
        return head == nullptr;
    }

    void add(K key, V value) {
        auto *node = new map_node();
        node->key = key;
        node->value = value;

        if (is_empty()) {
            head = node;
        } else {
            tail->next = node;
        }
        tail = node;
        size++;
    }

    void remove(K key) {
        if (is_empty()) {
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

    custom_iterator<V> get_iterator() override {
        V array[size];
        auto node = head;
        for (int i = 0; i < size; i++) {
            array[i] = node->value;
            node = node->next;
        }

        return custom_iterator<V>(size, array);
    }
};

struct custom_thread_info {
    void* thread = nullptr;
    custom_list<void *> lock;
    void *wait = nullptr;
};

bool operator==(const custom_thread_info& first, const custom_thread_info& second);

extern custom_map<void *, custom_thread_info *> custom_thread_pool;

custom_thread_info *custom_detector_get_thread_info(void* thread);

bool custom_detector_find_loop(custom_thread_info *tinfo, custom_list<void *> lock);

void custom_detector_check_deadlock(custom_thread_info *tinfo);

void report_deadlock();

void custom_mutex_pre_lock_interceptor(void *thread, void *mutex);

void custom_mutex_post_lock_interceptor(void *thread, void *mutex);

void custom_mutex_unlock_interceptor(void *thread, void *mutex);

#endif //SANITIZER_COMMON_CUSTOM_DEADLOCK_DETECTOR_H