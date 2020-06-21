#ifndef TSAN_TS_ACTION_H
#define TSAN_TS_ACTION_H


namespace __tsan {
    enum class TS_Action {
        CREATE_THREAD,
        COMPLETE_THREAD,
        LOCK_MUTEX,
        UNLOCK_MUTEX
    };
}


#endif //TSAN_TS_ACTION_H
