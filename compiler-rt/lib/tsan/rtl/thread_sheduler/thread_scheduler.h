#ifndef TSAN_THREAD_SCHEDULER_H
#define TSAN_THREAD_SCHEDULER_H

namespace __tsan {

    enum class TS_ThreadStatus {
        RUN,
        WAIT,
        UNKNOWN
    };

    enum class TS_Action {
        CREATE_THREAD,
        COMPLETE_THREAD,
        LOCK_MUTEX,
        UNLOCK_MUTEX
    };

    struct TS_ThreadInfo {
        void *tid = nullptr;
        TS_ThreadStatus status = TS_ThreadStatus::UNKNOWN;
    };

    struct TS_Point {
        TS_Action action;
        int currentTid;
        int nextTid;
    };

    struct ThreadScheduler {
    protected:
        virtual void synchronize(int tid, TS_Action action) = 0;

    public:
        virtual void synchronizeCreateThread(int tid) = 0;

        virtual void synchronizeCompleteThread(int tid) = 0;

        virtual void synchronizeLockMutex(int tid) = 0;

        virtual void synchronizeUnlockMutex(int tid) = 0;

        virtual void afterSynchronize(int tid) = 0;
    };

    ThreadScheduler &GetThreadScheduler();
}

#endif //TSAN_THREAD_SCHEDULER_H
