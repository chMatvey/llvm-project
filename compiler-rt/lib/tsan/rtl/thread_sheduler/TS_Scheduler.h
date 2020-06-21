#ifndef TSAN_THREAD_SCHEDULER_H
#define TSAN_THREAD_SCHEDULER_H


#include "TS_Action.h"

namespace __tsan {

    class TS_Scheduler {
    public:
        virtual void synchronize(int tid, TS_Action action) = 0;

        virtual void afterSynchronize(int tid) = 0;

        virtual void synchronizeCreateThread(int tid) = 0;

        virtual void synchronizeCompleteThread(int tid) = 0;

        virtual void synchronizeLockMutex(int tid) = 0;

        virtual void synchronizeUnlockMutex(int tid) = 0;
    };

    TS_Scheduler &GetThreadScheduler();
}


#endif //TSAN_THREAD_SCHEDULER_H
