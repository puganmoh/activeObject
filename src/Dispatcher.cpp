// =====================================================================================================================
// @file Dispatcher.cpp
// @brief Dispatcher to schedule and execute the action request
//
// =====================================================================================================================

#include "Dispatcher.hpp"

Dispatcher::Dispatcher() : exitThread_(false),
                           initializeStatus_(InitState::UNKNOWN)
{
    // Get thread from the pool and launch the thread entry point
    std::shared_ptr<Runnable> handler{this};

    initializeStatus_ = Thread::getInstance()->start(handler, tid_);
    if(initializeStatus_ == InitState::INIT)
    {
        initialize_ = std::promise<bool>();
    }
}

Dispatcher::~Dispatcher()
{
}

int Dispatcher::start(NotificationBase::Ptr notification)
{
     // Wait until thread availability
     if(initializeStatus_ == InitState::INIT)
     {
         // To Do: Add timeout
         initializeStatus_ = initialize_.get_future().get();
     }

     if(initializeStatus_ == InitState::READY)
     {
         // enqueue to the dispatcher queue
         mtx_.lock();
         commandQueue_.push(notification);
         mtx_.unlock();

         // Notify the event
         event_.notify_one();
     }

     return initializeStatus_;
}

int Dispatcher::cancel()
{
     mtx_.lock();
     exitThread_ = true;
     mtx_.unlock();

     // release allocated thread back to the pool
     event_.notify_one();

     return Thread::getInstance()->cancel(tid_);
}

void Dispatcher::run()
{
     do
     {
          std::unique_lock<std::mutex> lck(mtx_);

          if(initializeStatus_ == InitState::INIT)
          {
              initialize_.set_value(InitState::READY);
          }

	  // wait for the event
          event_.wait(lck);

          // lock automotically re-acquired, hence continue
	  // with queue operations.
          if(!commandQueue_.empty())
          {
              // Retrieve the action request
              auto executePtr = commandQueue_.front();
	      
              // Dequeue the action request
              commandQueue_.pop();

              // Capture action execution status for warning
              auto status = false;
              
	      if(executePtr)
                   status = (*executePtr)->execute();
          }

     }while(!exitThread_);
}
