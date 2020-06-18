//
// Created by matvey on 6/14/20.
//

#ifndef SANITAZER_TS_ENTRY_H
#define SANITAZER_TS_ENTRY_H


namespace __tsan::ts::util {

    template <typename K, typename V>
    class TS_Entry {
    private:
        K key;
        V value;

    public:
        TS_Entry() {}

        TS_Entry(K key, V value) : key(key), value(value) {}

        K getKey() const {
            return key;
        }

        V getValue() const {
            return value;
        }
    };
}


#endif //SANITAZER_TS_ENTRY_H
