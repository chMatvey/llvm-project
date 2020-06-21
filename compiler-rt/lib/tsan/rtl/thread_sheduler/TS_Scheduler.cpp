#include "TS_Scheduler.h"
#include "util/TS_Queue.h"
#include "TS_Action.h"
#include "TS_Point.h"
#include "TS_ThreadInfo.h"
#include "TS_ThreadStatus.h"
#include "util/TS_FIleManager.h"
#include "lexer/TS_Lexer.h"
#include "lexer/TS_Token.h"
#include "parser/TS_Parser.h"

#include <rtl/tsan_rtl.h>
#include <sanitizer_common/sanitizer_allocator_internal.h>
#include <sanitizer_common/sanitizer_placement_new.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

using namespace __tsan::ts::util;
using namespace __tsan::ts::lexer;
using namespace __tsan::ts::parser;

namespace __tsan {
    TS_ThreadInfo threadInfo[65536] = {};

    int threadCount = 1;

    class TS_DeterministicScheduler : public TS_Scheduler {

    private:
        TS_Queue<TS_Point *> *actions = nullptr;

    public:
        TS_DeterministicScheduler() {}

        TS_DeterministicScheduler(TS_Queue<TS_Point *> *actions) {
            this->actions = actions;
        }

        void synchronize(int tid, TS_Action action) override {
            if (actions == nullptr || actions->isEmpty()) {
                return;
            }

            if (action == TS_Action::CREATE_THREAD) {
                threadCount++;
            }

            auto currentAction = actions->getCurrent();

            if (currentAction->currentTid == tid && currentAction->action == action) {
                for (int i = 0; i < threadCount; i++) {
                    if (i == currentAction->nextTid || currentAction->nextTid == -1) {
                        //__atomic_store_n(&threadInfo[i].status, TS_ThreadStatus::RUN, __ATOMIC_SEQ_CST);
                        threadInfo[i].status = TS_ThreadStatus::RUN;
                    } else {
                        //__atomic_store_n(&threadInfo[i].status, TS_ThreadStatus::WAIT, __ATOMIC_SEQ_CST);
                        threadInfo[i].status = TS_ThreadStatus::WAIT;
                    }
                }
                actions->next();
            }
        }

        void afterSynchronize(int tid) override {
            if (actions != nullptr && actions->isEmpty()) {
                return;
            }

            while (__atomic_load_n(&threadInfo[tid].status, __ATOMIC_SEQ_CST) == TS_ThreadStatus::WAIT) {
                internal_sched_yield();
            }
        }

        void synchronizeCreateThread(int tid) override {
            synchronize(tid, TS_Action::CREATE_THREAD);
        }

        void synchronizeCompleteThread(int tid) override {
            synchronize(tid, TS_Action::COMPLETE_THREAD);
        }

        void synchronizeLockMutex(int tid) override {
            synchronize(tid, TS_Action::LOCK_MUTEX);
        }

        void synchronizeUnlockMutex(int tid) override {
            synchronize(tid, TS_Action::UNLOCK_MUTEX);
        }
    };

    TS_Scheduler *threadScheduler = nullptr;

    TS_Scheduler &GetThreadScheduler() {
        Printf("Path to test scenario %s", flags()->ts_path);
        if (threadScheduler == nullptr) {
            auto *scheduler = static_cast<TS_DeterministicScheduler *>(InternalCalloc(1, sizeof(TS_DeterministicScheduler)));
            if (strcmp(flags()->ts_path, "") == 0) {
                scheduler = new TS_DeterministicScheduler();
            } else {
                TS_Lexer lexer;
                TS_Parser parser;

                const char *fileContent = __tsan::ts::util::TS_FIleManager::readFile(flags()->ts_path);
                auto tokens = lexer.scan(fileContent);
                auto *points = parser.parse(tokens);

                scheduler = new TS_DeterministicScheduler(points);
            }

            threadScheduler = scheduler;
        }

        return *threadScheduler;
    }
}