#ifndef SANITAZER_TS_ITERABLE_H
#define SANITAZER_TS_ITERABLE_H


#include "TS_Iterator.h"

namespace __tsan::ts::util {

    template <typename T>
    class TS_Iterable {
    public:
        virtual TS_Iterator<T> iterator() {}
    };
}


#endif //SANITAZER_TS_ITERABLE_H
