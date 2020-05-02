#ifndef RTL_STRUCTURES_H
#define RTL_STRUCTURES_

template <typename T>
class TS_Queue {
private:
    class queueNode {
    public:
        T key;
        queueNode *next = nullptr;
    };

    queueNode *head = nullptr;
    queueNode *tail = nullptr;
    int size = 0;

public:
    bool isEmpty() {
        return size == 0;
    }

    void add(T key) {
        auto *node = new queueNode();
        node->key = key;

        if (isEmpty()) {
            head = node;
        } else {
            tail->next = node;
        }
        tail = node;
        size++;
    }

    T getCurrent() {
        return head->key;
    }

    int Count() {
        return size;
    }

    void next() {
        head = head->next;
        size--;
    }
};
#endif //RTL_STRUCTURES_H
