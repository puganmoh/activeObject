// =====================================================================================================================
// @file Notification.hpp
// @brief Notification class for store the action metadata.
//
// =====================================================================================================================

#ifndef _INCLUDE_NOTIFICATION_H_
#define _INCLUDE_NOTIFICATION_H_

#include "Headers.hpp"

class NotificationBase
{
    public:
        using Ptr = std::shared_ptr<NotificationBase*>;

        /**
         * @brief execute(): Execute the registered action.
         * Helper method to execute the stored action request.
         * @param None
         * @return True, false if notification not executed.
         **/
	virtual bool execute() = 0;

        /**
         * @brief ~NotificationBase(): Destructor.
         * Deinitialize/release the instantiated resources.
         * @param None
         * @return None.
         **/
	virtual ~NotificationBase()
        {}
};

template<class ResultType, class ArgType, class InstanceType>
class Notification : public NotificationBase
{
private:
   typedef ResultType (InstanceType::*Callback)(const ArgType&);
   typedef std::promise<ResultType> NotificationResult;

   InstanceType *instance_;
   Callback callback_;
   ArgType arg_;
   NotificationResult result_;

public:
   /**
    * @brief Notification(): Constructor.
    * Initialize the action notification.
    * @param Instance of the target.
    * @param Actor to be stored.
    * @param Arguments to pass into the action.
    * @param Result of the action.
    * @return None
    **/
   Notification(InstanceType *instance, Callback callback, const ArgType &arg, NotificationResult result) :
                                   instance_(instance), callback_(callback), arg_(arg), result_(std::move(result))
   {}

   bool execute()
   {
       bool status = false;

       if(instance_)
       {
           auto ret = (instance_->*callback_)(arg_);
           result_.set_value(ret);
           status = true;
       }

       return status;
   }

};

#endif
