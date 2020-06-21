#ifndef TSAN_TS_THREADINFO_H
#define TSAN_TS_THREADINFO_H


#include "TS_ThreadStatus.h"

namespace __tsan {
    struct TS_ThreadInfo {
        void *tid = nullptr;
        TS_ThreadStatus status = TS_ThreadStatus::UNKNOWN;
    };
}


#endif //TSAN_TS_THREADINFO_H
