// =====================================================================================================================
// @file Dispatcher.hpp
// @brief Dispatcher to schedule and execute the action request
//
// =====================================================================================================================

#ifndef _INCLUDE_DISPATCHER_H_
#define _INCLUDE_DISPATCHER_H_

#include "Headers.hpp"
#include "Notification.hpp"
#include "Thread.hpp"

enum InitState
{
    UNKNOWN = -1,      // Default state
    INIT = 0,          // State where dispatcher fetches thread successfully from the pool
    READY = 1          // State where dispatcher thread handler ready for accepting work
};

class Dispatcher : public Runnable
{
public:

  /**
   * @brief Dispatcher(): Constructor.
   * Initialize the dispatcher.
   * @param None
   * @return None
   **/
  Dispatcher();

  /**
   * @brief Dispatcher(): Destructor.
   * Deinitialize the dispatcher.
   * @param None
   * @return None
   **/
  ~Dispatcher();

  /**
   * @brief start(): Start the action request for execution.
   * Queue and set the request for execution.
   * @param Pointer to the method attributes.
   * @return 0 on success, on error, -1.
   **/
  int start(NotificationBase::Ptr notification);

  /**
   * @brief cancel(): Cancel the operation.
   * Clean up resources created by the dispatcher.
   * @param None.
   * @return 0 on success, on error, -1.
   **/
  int cancel();

  /**
   * @brief run(): Entry point to the dispatcher.
   * Listen and execute the action request.
   * @param None.
   * @return None.
   **/
  void run();

private:
  std::queue<NotificationBase::Ptr> commandQueue_;
  pthread_t tid_;
  std::condition_variable event_;
  std::mutex mtx_;

  bool exitThread_;
  std::promise<bool> initialize_;
  int initializeStatus_;
};

#endif
