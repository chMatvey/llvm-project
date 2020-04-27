#include "custom_deadlock_detector.h"

bool operator==(const custom_thread_info& first, const custom_thread_info& second) {
    return first.thread == second.thread;
}

custom_map<void *, custom_thread_info *> custom_thread_pool;

void *custom_pool_mutex_attrib = nullptr;

void *custom_pool_mutex = nullptr;

custom_thread_info *custom_detector_get_thread_info(void* thread) {
    auto tinfo = custom_thread_pool.search(thread);

    if (tinfo == nullptr) {
        tinfo = new custom_thread_info();
        tinfo->thread = thread;
        custom_thread_pool.add(thread, tinfo);
    }

    return tinfo;
}

bool custom_detector_find_loop(custom_thread_info *tinfo, custom_list<void *> lock) {
    if (!tinfo->waiting_mutex) {
        return false;
    }

    //Other thread try lock mutex that locked current thread
    if (lock.search(tinfo->waiting_mutex)) {
        return true;
    }

    //Find thread that lock mutex that current thread needs
    auto thread_iterator = custom_thread_pool.get_iterator();
    while (thread_iterator.has_next()) {
        auto thread_info = thread_iterator.next();
        auto mutex_iterator = thread_info->locked_mutexes.get_iterator();

        while (mutex_iterator.has_next()) {
            auto mutex = mutex_iterator.next();

            if (mutex == tinfo->waiting_mutex) {
                return custom_detector_find_loop(thread_info, lock);
            }
        }
    }

    return false;
}

void custom_detector_check_deadlock(custom_thread_info *tinfo) {
    auto thread_iterator = custom_thread_pool.get_iterator();

    while (thread_iterator.has_next()) {
        auto thread_info = thread_iterator.next();
        auto mutex_iterator = thread_info->locked_mutexes.get_iterator();

        while (mutex_iterator.has_next()) {
            auto mutex = mutex_iterator.next();

            if (mutex == tinfo->waiting_mutex &&
                (tinfo->thread == thread_info->thread || custom_detector_find_loop(thread_info, tinfo->locked_mutexes))) {
                report_deadlock();
            }
        }
    }
}

void report_deadlock() {
    //need to implemented
}

void custom_mutex_pre_lock_interceptor(void *thread, void *mutex) {
    custom_thread_info *tinfo = custom_detector_get_thread_info(thread);
    tinfo->waiting_mutex = mutex;
    custom_detector_check_deadlock(tinfo);
}

void custom_mutex_post_lock_interceptor(void *thread, void *mutex) {
    custom_thread_info *tinfo = custom_detector_get_thread_info(thread);
    tinfo->waiting_mutex = nullptr;
    tinfo->locked_mutexes.add(mutex);
}

void custom_mutex_unlock_interceptor(void *thread, void *mutex) {
    custom_thread_info *tinfo = custom_detector_get_thread_info(thread);
    tinfo->locked_mutexes.remove(mutex);
}