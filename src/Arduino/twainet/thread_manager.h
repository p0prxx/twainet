#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "thread.h"
#include "include/singleton.h"
#include "std/list.hpp"
#include "std/vector.hpp"

extern "C" {
#include <ets_sys.h>
#include <os_type.h>
#include <cont.h>
#include <debug.h>
}
    
#define THREAD_START_ID 2
#define QUEUE_SIZE      1
#define THREAD_MAX      3

struct ThreadDescription
{
    cont_t m_cont __attribute__ ((aligned (16)));
    os_event_t m_loop_queue[QUEUE_SIZE];
    unsigned int m_id;
    Thread* m_thread;
    enum {
        ABSENT,
        CREATED,
        RUNNING,
        WAITING,
        STOPPED,
        STOP_PENDING
    } m_state;
};

extern ThreadDescription g_threadDesks[THREAD_MAX];

class ThreadManager : public Singleton<ThreadManager>
{
protected:
	friend class Singleton<ThreadManager>;
	ThreadManager();
	virtual ~ThreadManager();
public:
	void AddThread(Thread* thread);
	void RemoveThread(Thread* thread);
    void CheckThreads();
    unsigned int GetCurrentThreadId();
protected:
    friend void thread_wrapper();
    void RunThreadFunc(Thread* thread);
};

#endif/*THREAD_MANAGER_H*/