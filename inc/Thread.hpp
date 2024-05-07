// =====================================================================================================================
// @file Thread.hpp
// @brief Wrapper class for posix threads.
//
// =====================================================================================================================

#ifndef _INCLUDE_THREAD_H_
#define _INCLUDE_THREAD_H_

#include <pthread.h>
#include "Headers.hpp"
#include "Runnable.hpp"

// Maximum thread count supported by the pool
#define MAX_THREAD_COUNT 10

enum ThreadState
{
    IDLE = 0,       // Thread default state
    RUN,            // Thread is assigned with target handler
    EXIT,           // Set to thread handler completion
    ERROR           // Thread encountered with an error
};

struct ThreadData
{
    pthread_t threadId;
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    ThreadState state;
    std::shared_ptr<Runnable> handler;
};

/**
 * @brief threadEntry(): Handler for newly created thread object.
 * It runs the run() method on the thread object passed to it.
 * NOTE: It has a C linkage.
 * @param Thread data on which the handler process.
 * @return -1 on success/failure.
 **/
extern "C" void *threadEntry(void *input);

class Thread
{
     public:
        /**
         * @brief Thread(): Constructor.
         * Create threads and make it ready for accepting work.
         * @param None
         * @return None
         **/
       Thread();

        /**
         * @brief Thread(): Destructor.
         * Destroy the thread pool.
         * @param None
         * @return None
         **/
       ~Thread();

        /**
         * @brief getInstance(): Provide instance of the thread pool.
         * Instantiate and provide instance of the thread pool.
         * @param None
         * @return Thread pool instance, on error, nullptr.
         **/
       static Thread* getInstance();

        /**
         * @brief start(): Start the thread with target object.
         * Get the available thread from pool and launch target for execution.
         * @param Target object handler to run.
         * @param Id of the assigned thread.
         * @return 0 on success, on error, -1.
         **/
       int start(std::shared_ptr<Runnable> task, pthread_t &tid);

        /**
         * @brief cancel(): Remove the target object from thread execution.
         * Cancel the running thread and release the thread back to the pool.
         * @param Id of the assigned thread.
         * @return 0 on success, on error, -1.
         **/
       int cancel(const pthread_t &tid);

        /**
         * @brief cancelAll(): Remove all target object from respective thread execution.
         * Cleanup the running threads and release the thread resources back to system.
         * back to system.
         * @param None
         * @return 0 on success, on error, -1.
         **/
       int cancelAll();

     private:
       Thread(const Thread&) = delete;
       Thread& operator=(const Thread&) = delete;

     private:
        ThreadData threadList[MAX_THREAD_COUNT];
        static Thread* instance;
};

#endif
