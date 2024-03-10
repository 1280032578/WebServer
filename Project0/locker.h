#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

// 线程同步机制封装类

// 互斥锁

class locker
{
public:
    // 构造函数
    locker()
    {
        // 初始化互斥量
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    // 析构函数
    ~locker()
    {
        // 释放互斥量资源
        pthread_mutex_destroy(&m_mutex);
    }
    // 加锁
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    // 解锁
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    // 返回互斥变量
    pthread_mutex_t *get()
    {
        return &m_mutex;
    }

private:
    // 创建互斥量
    pthread_mutex_t m_mutex;
};

// 条件变量
// 当满足条件时，才执行，不是锁，配合互斥量使用
// ex:当有生产者生产时，通知消费者消费，否则等待

class cond
{
public:
    // 构造函数
    cond()
    {
        if (pthread_cond_init(&m_cond, NULL) != 0)
        {
            throw std::exception();
        }
    }
    // 析构函数
    ~cond()
    {
        pthread_cond_destroy(&m_cond);
    }
    // 等待，调用了该函数，线程会阻塞
    bool wait(pthread_mutex_t *m_mutex)
    {
        int ret = 0;
        ret = pthread_cond_wait(&m_cond, m_mutex);
        return ret == 0;
    }
    // 等待多长时间，调用了这个函数，线程会阻塞，直到指定的时间结束
    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        return ret == 0;
    }
    // 唤醒一个或者多个等待的线程
    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }
    // 唤醒所有的等待的线程
    bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }

private:
    pthread_cond_t m_cond;
};

// 信号量类

class sem
{
public:
    sem()
    {
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            throw std::exception();
        }
    }
    sem(int num)
    {
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }
    ~sem()
    {
        sem_destroy(&m_sem);
    }
    // 等待信号量
    bool wait()
    {
        // 功能：对信号量加锁，调用一次对信号量的值-1，如果值为0，就阻塞
        return sem_wait(&m_sem) == 0;
    }
    // 增加信号量
    bool post()
    {
        // 功能：对信号量解锁，调用一次对信号量的值+1
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};
#endif