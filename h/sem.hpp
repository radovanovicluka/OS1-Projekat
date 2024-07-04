
#ifndef _semaphore_
#define _semaphore_

#include "../h/tcb.hpp"
#include "../h/list.hpp"

class sem {
public:

    using sem_t = sem*;

    int signal();
    int wait();
    int trywait();
    int close();

    static int sem_open( sem_t* , unsigned );

private:
    int value;
    List<TCB> blocked;
    bool open;

    sem ( int val = 1 );
    ~sem ();
};

#endif
