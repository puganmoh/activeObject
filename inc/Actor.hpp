// =====================================================================================================================
// @file Actor.hpp
// @brief Actor class to generate action request and assign to the dispatcher.
//
// =====================================================================================================================

#ifndef _INCLUDE_ACTOR_H_
#define _INCLUDE_ACTOR_H_

#include "Headers.hpp"
#include "Notification.hpp"

template<class ResultType, class ArgType, class InstanceType>
class Actor
{
public:
   typedef ResultType (InstanceType::*Callback)(const ArgType&);
   typedef std::future<ResultType> HandlerResult;
   typedef Notification<ResultType, ArgType, InstanceType> NotificationType;

  /**
   * @brief Actor(): Constructor.
   * Initialize action class.
   * @param Instance of the target class
   * @param Method from the target class
   * @return none
   **/
  explicit Actor(InstanceType *instanceType, Callback method) : instanceType_(instanceType),
                                                                 method_(method)
  {
  }

  /**
   * @brief Actor(): Destructor.
   * Destroy the action.
   * @param Instance of the target class
   * @param Method from the target class
   * @return none
   **/
  ~Actor()
  {
  }

  /**
   * @brief Actor(): Actor method.
   * Action function operator overloaded.
   * @param Instance of the target class
   * @param Method from the target class
   * @return none
   **/
  HandlerResult operator()(const ArgType &param)
  {
      // reset the result
      result_ = std::promise<ResultType>();

      auto handlerResult = result_.get_future();
      auto dispatch = std::make_shared<NotificationBase*>(new NotificationType(instanceType_, method_, param, std::move(result_)));

      // call the dispatcher to enqueue the action request
      if(instanceType_)
          instanceType_->start(dispatch);

      return handlerResult;
  }

  /**
   * @brief Actor(): Copy constructor.
   * Create copy of the target object.
   * @param Target object
   * @return none
   **/
  Actor(const Actor &other): instanceType_(other.instanceType_),
                               method_(other.method_)
  {}

  /**
   * @brief Actor(): Assignment operator.
   * Assigns target to local object.
   * @param Target object
   * @return none
   **/
  Actor& operator=(const Actor &other)
  {
      Actor temp(other);
      swap(temp);
      return *this;
  }

  /**
   * @brief swap(): swap the object.
   * Swap target to local object.
   * @param Target object
   * @return none
   **/
  void swap(Actor &other) noexcept
  {
      std::swap(instanceType_, other.instanceType_);
      std::swap(method_, other.method_);
  }

private:
   Actor() = delete;

   InstanceType *instanceType_;
   Callback method_;
   std::promise<ResultType> result_;
};

#endif
