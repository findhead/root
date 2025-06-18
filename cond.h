#ifndef COND_H
#define COND_H

#include <pthread.h>
#include <ctime>
#include "locker.h"

// 条件变量类
class cond {
public:
    cond() {
        if (pthread_cond_init(&m_cond, NULL) != 0) {
            throw std::exception();
        }
    }

    ~cond() {
        pthread_cond_destroy(&m_cond);
    }

    // 等待条件变量
    bool wait(locker& mutex) {
        return pthread_cond_wait(&m_cond, mutex.get()) == 0;
    }

    // 带超时的等待
    bool timewait(locker& mutex, struct timespec t) {
        return pthread_cond_timedwait(&m_cond, mutex.get(), &t) == 0;
    }

    // 唤醒一个等待线程
    bool signal() {
        return pthread_cond_signal(&m_cond) == 0;
    }

    // 唤醒所有等待线程
    bool broadcast() {
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    pthread_cond_t m_cond;
};

#endif // COND_H
