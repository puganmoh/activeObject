// =====================================================================================================================
// @file Runnable.hpp
// @brief Interface for thread entry points.
//
// =====================================================================================================================

#ifndef _INCLUDE_RUNNABLE_H_
#define _INCLUDE_RUNNABLE_H_

#include "Headers.hpp"

class Runnable
{
    public:
        /**
         * @brief Runnable(): Destructor.
         * Destroy the instance of runnable.
         * @param None
         * @return None
         **/
         virtual ~Runnable() {}

        /**
         * @brief start(): Interface for thread entry point.
         * Implement by classes that provide an entry point for a thread.
         * @param None
         * @return None
         **/
         virtual void run() = 0;
};

#endif
