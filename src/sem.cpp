#include "../h/sem.hpp"

int sem::sem_open( sem_t* handle, unsigned init ) {
    *handle = new sem( init );
    return 0;
}

sem::sem(int val) {
    value = val;
    open = true;
}

int sem::signal() {
    if ( !open ) {
        return -1;
    }

    TCB *t;
    if ( ++value <= 0 ) {
        t = blocked.getFirst();
        t->unblock();
        Scheduler::put( t );
    }
    return 0;
}

int sem::wait() {
    if ( !open ) {
        return -1;
    }

    if ( --value < 0 ) {
        TCB::running->block();
        blocked.addLast( TCB::running );

        TCB::dispatch();
//        thread_dispatch();

        if ( !open ) {
            return -2;
        }
    }
    return 0;
}

int sem::trywait() {
    if ( value <= 0 ) {
        return 1;
    }
    else {
        value--;
        return 0;
    }
}

int sem::close() {
    if ( !open ) {
        return -1;
    }

    open = false;

    while ( blocked.peekFirst() != nullptr ) {
        TCB *t = blocked.getFirst();
        t->unblock();
        Scheduler::put( t );
    }
    return 0;
}

sem::~sem() {
    if ( open ) { close(); }
}
