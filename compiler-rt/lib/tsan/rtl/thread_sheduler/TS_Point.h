#ifndef TSAN_TS_POINT_H
#define TSAN_TS_POINT_H


#include "TS_Action.h"

namespace __tsan {
    struct TS_Point {
        TS_Action action;
        int currentTid;
        int nextTid;

        TS_Point() {}

        TS_Point(TS_Action action, int currentTid, int nextTid) : action(action), currentTid(currentTid),
                                                                  nextTid(nextTid) {}
    };
}


#endif //TSAN_TS_POINT_H
