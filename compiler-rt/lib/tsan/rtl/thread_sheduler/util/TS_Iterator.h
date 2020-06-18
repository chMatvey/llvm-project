#ifndef SANITAZER_TS_ITERATOR_H
#define SANITAZER_TS_ITERATOR_H


namespace __tsan::ts::util {

    template <typename T>
    class TS_Iterator {
    private:
        int size;
        int current_index = 0;
        T *array;

    public:
        TS_Iterator(int size, T *array) : size(size), array(array) {}

        bool hasNext() {
            return current_index != size;
        }

        T next() {
            T result = array[current_index];
            current_index++;

            return result;
        }
    };
}


#endif //SANITAZER_TS_ITERATOR_H
