#ifndef SANITAZER_TS_COMPARATOR_H
#define SANITAZER_TS_COMPARATOR_H


namespace __tsan::ts::util {
    template <typename T>
    class TS_Comparator {
    public:
        virtual int compare(T a, T b) = 0;
    };
}


#endif //SANITAZER_TS_COMPARATOR_H
