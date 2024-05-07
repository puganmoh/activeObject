// =====================================================================================================================
// @file Dispatcher.cpp
// @brief Dispatcher to schedule and execute the action request
//
// =====================================================================================================================

#include "Dispatcher.hpp"
#include "Actor.hpp"

class Demo : public Dispatcher
{
    public:
        Demo() : start(this, &Demo::doStart);
    public:
        typedef Cammgr::Action<bool, bool, Demo> Start;

        Start start;

    public:
        bool doStart(bool in)
	{
	}	
};


int main(int argc, char *argv[])
{
    Demo demo;
    demo.start(); 
}
