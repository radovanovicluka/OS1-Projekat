
#ifndef _tcb_
#define _tcb_

#include "../lib/hw.h"
#include "../h/scheduler.hpp"

class TCB;
class Thread;

using thread_t = TCB*;

class TCB {
public:

    static void wrapper();

    using Body = void (*)(void*);

    static int createThread( thread_t* handle, Body body, void* args, void* stack ) {
        *handle = (thread_t) new TCB( body, args, stack );
        if ( body != nullptr ) Scheduler::put( *handle );
        return 0;
    }

    static int createNoStart( thread_t* handle, Body body, void* args, void* stack ) {
        *handle = (thread_t) new TCB( body, args, stack );
        return 0;
    }

    static void start ( TCB* thread ) {
        if ( thread->body != nullptr ) Scheduler::put( thread );
    }

    static void dispatch();
    static void exit();

    static TCB* running;

    bool isFinished() const { return finished; }

    void setFinished(bool finished);

    void block() { blocked = true; }
    void unblock() { blocked = false; }

    bool isBlocked() const { return blocked; }

    ~TCB() {
        delete stack;
    }

private:
    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    uint64 *stack;
    Context context;
    void *args;
    bool finished;
    bool blocked;

    TCB( Body body, void* args, void* stack  );

//    static void wrapper();

    static void popSppSpie();

    static void yield( Context *, Context * );

    friend Thread;
};

#endif
