#ifndef TSAN_TS_THREADSTATUS_H
#define TSAN_TS_THREADSTATUS_H


namespace __tsan {
    enum class TS_ThreadStatus {
        RUN,
        WAIT,
        UNKNOWN
    };
}


#endif //TSAN_TS_THREADSTATUS_H
