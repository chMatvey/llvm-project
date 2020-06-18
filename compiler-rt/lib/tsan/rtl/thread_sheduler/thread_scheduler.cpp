#include "thread_scheduler.h"
#include "util/TS_Queue.h"

#include <rtl/tsan_rtl.h>
#include <sanitizer_common/sanitizer_allocator_internal.h>
#include <sanitizer_common/sanitizer_placement_new.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

using  namespace __tsan::ts::util;

namespace __tsan {
    TS_ThreadInfo threadInfo[65536] = {};

    int threadCount = 1;

    struct BaseThreadScheduler : ThreadScheduler {
        BaseThreadScheduler() {
            actions.add({TS_Action::CREATE_THREAD, 0, 0});
            actions.add({TS_Action::CREATE_THREAD, 0, 1});
            actions.add({TS_Action::LOCK_MUTEX, 1, 2});
            actions.add({TS_Action::LOCK_MUTEX, 2, -1});
        }

    private:
        TS_Queue<TS_Point> actions;

        void synchronize(int tid, TS_Action action) override {
            if (actions.isEmpty()) {
                return;
            }

            if (action == TS_Action::CREATE_THREAD) {
                threadCount++;
            }

            TS_Point currentAction = actions.getCurrent();

            if (currentAction.currentTid == tid && currentAction.action == action) {
                for (int i = 0; i < threadCount; i++) {
                    if (i == currentAction.nextTid || currentAction.nextTid == -1) {
                        //__atomic_store_n(&threadInfo[i].status, TS_ThreadStatus::RUN, __ATOMIC_SEQ_CST);
                        threadInfo[i].status = TS_ThreadStatus::RUN;
                    } else {
                        //__atomic_store_n(&threadInfo[i].status, TS_ThreadStatus::WAIT, __ATOMIC_SEQ_CST);
                        threadInfo[i].status = TS_ThreadStatus::WAIT;
                    }
                }
                actions.next();
            }
        }

    public:
        void synchronizeCreateThread(int tid) {
            synchronize(tid, TS_Action::CREATE_THREAD);
        }

        void synchronizeCompleteThread(int tid) {
            synchronize(tid, TS_Action::COMPLETE_THREAD);
        }

        void synchronizeLockMutex(int tid) {
            synchronize(tid, TS_Action::LOCK_MUTEX);
        }

        void synchronizeUnlockMutex(int tid) {
            synchronize(tid, TS_Action::UNLOCK_MUTEX);
        }

        void afterSynchronize(int tid) {
            while (__atomic_load_n(&threadInfo[tid].status, __ATOMIC_SEQ_CST) == TS_ThreadStatus::WAIT) {
                internal_sched_yield();
            }
        }
    };

    ThreadScheduler *threadScheduler = nullptr;

    ThreadScheduler &GetThreadScheduler() {
        if (threadScheduler == nullptr) {
            auto *scheduler = static_cast<BaseThreadScheduler *>(InternalCalloc(1, sizeof(BaseThreadScheduler)));
            new(scheduler) BaseThreadScheduler;
            threadScheduler = scheduler;
        }

        return *threadScheduler;
    }
}