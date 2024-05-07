// =====================================================================================================================
// @file Thread.cpp
// @brief Thread pool to assign thread for target object.
//
// =====================================================================================================================

#include "Thread.hpp"

Thread* Thread::instance = nullptr;

Thread* Thread::getInstance()
{
    if(!instance)
    {
        instance = new Thread();
    }

    return instance;
}

// Constructor
Thread::Thread()
{
    for(int index = 0; index < MAX_THREAD_COUNT ; index++)
    {
        pthread_mutex_init(&threadList[index].mutex, nullptr);
        pthread_cond_init(&threadList[index].condition, nullptr);
        threadList[index].state = ThreadState::IDLE;
        threadList[index].handler = nullptr;

        if(pthread_create(&threadList[index].threadId, NULL, threadEntry, (void*)(&threadList[index])) != 0)
        {
            std::cout << __func__ << "\tEnd created thread = " << threadList[index].threadId << std::endl;
            threadList[index].state = ThreadState::ERROR;
        }
     }
}

// Destructor
Thread::~Thread()
{
}

int Thread::start(std::shared_ptr<Runnable> task, pthread_t &tid)
{
    int status = -1;

    // Sanity check
    if(task)
    {
        for(int index = 0; index < MAX_THREAD_COUNT; index++)
        {
            if((threadList[index].state == ThreadState::IDLE) &&
               (threadList[index].state != ThreadState::ERROR))
            {
                pthread_mutex_lock(&threadList[index].mutex);

                // Assign target thread object with a thread from the pool
                threadList[index].handler = task;
                tid = threadList[index].threadId;
                threadList[index].state = ThreadState::RUN;

                pthread_cond_signal(&threadList[index].condition);
                pthread_mutex_unlock(&threadList[index].mutex);

                status = 0;
                break;
           }
        }

        if(status != 0)
            std::cout << __func__ << "Worker thread unavailable from the pool" << std::endl;
    }
    else
    {
        std::cout << __func__ << "Target thread object is invalid" << std::endl;
    }

    return status;
}

int Thread::cancel(const pthread_t &tid)
{
    bool status = -1;

    for(int index = 0; index < MAX_THREAD_COUNT; index++)
    {
        if((threadList[index].threadId == tid) &&
           (threadList[index].state == ThreadState::RUN))
        {
            pthread_mutex_lock(&threadList[index].mutex);

            // reset the thread state
            threadList[index].state = ThreadState::IDLE;
            pthread_cond_signal(&threadList[index].condition);

            pthread_mutex_unlock(&threadList[index].mutex);

            status = 0;
            break;
        }
    }

    if(status != 0)
        std::cout << tid << "thread id not found in the thread pool" << std::endl;

    return status;
}

int Thread::cancelAll()
{
    int status = 0;

    for(int index = 0; index < MAX_THREAD_COUNT ; index++)
    {
        if(threadList[index].state != ThreadState::ERROR)
        {
            pthread_mutex_lock(&threadList[index].mutex);

            // Set and notify the handler to exit
            threadList[index].state = ThreadState::EXIT;
            pthread_cond_signal(&threadList[index].condition);

            pthread_mutex_unlock(&threadList[index].mutex);

            // Wait for thread exit
            status |= pthread_join(threadList[index].threadId , NULL);
        }

        pthread_mutex_destroy(&threadList[index].mutex);
        pthread_cond_destroy(&threadList[index].condition);
    }

    return (status == 0) ? status : -1;
}

// Thread Handler
void *threadEntry(void *handlerObj)
{
    auto handlerData = static_cast<ThreadData*>(handlerObj);

    // Sanity check
    if(handlerData == nullptr)
    {
       std::cout << __func__ << pthread_self() <<  "\t thread data is null" << std::endl;
       handlerData->state == ThreadState::ERROR;
       return (void*)-1;
    }

    do
    {
        pthread_mutex_lock(&handlerData->mutex);

        if((handlerData->handler) ||
           (handlerData->state == ThreadState::IDLE))
        {
            pthread_cond_wait(&handlerData->condition, &handlerData->mutex);
        }

        if((handlerData->state == ThreadState::RUN) &&
           (handlerData->handler))
        {
            // Start the target thread object handler
            handlerData->handler->run();
        }

        pthread_mutex_unlock(&handlerData->mutex);

    } while(handlerData->state != ThreadState::EXIT);

    return (void*)-1;
}
