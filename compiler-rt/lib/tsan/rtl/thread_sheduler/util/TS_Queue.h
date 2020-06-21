#ifndef SANITAZER_TS_QUEUE_H
#define SANITAZER_TS_QUEUE_H


namespace __tsan::ts::util {
    template <typename T>
    class TS_Queue {
    private:
        class TS_QueueNode {
        public:
            T key;
            TS_QueueNode *next = nullptr;
        };

        TS_QueueNode *head = nullptr;
        TS_QueueNode *tail = nullptr;
        int size = 0;

    public:
        bool isEmpty() {
            return size == 0;
        }

        void add(T key) {
            auto *node = new TS_QueueNode();
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

        T getCurrent() {
            return head->key;
        }

        int Count() {
            return size;
        }

        void next() {
            TS_QueueNode *old = head;
            head = head->next;
            size--;
            delete old;
        }

        T getLast() {
            return tail->key;
        }
    };
}


#endif //SANITAZER_TS_QUEUE_H