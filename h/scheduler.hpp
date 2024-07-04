
#ifndef _scheduler_
#define _scheduler_

#include "list.hpp"

class TCB;

class Scheduler {
private:
    static List<TCB> threadQueue;

public:
    static void put( TCB* tcb );

    static TCB* get();
};

#endif