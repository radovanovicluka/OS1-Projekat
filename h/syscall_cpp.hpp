#ifndef _syscall_cpp_
#define _syscall_cpp_

#include "syscall_c.h"

class Thread {
public:
    Thread (void (*body)(void*), void* arg);
    virtual ~Thread ();
    int start ();
    static void dispatch ();
    static int sleep (time_t);

    static void runWrapper( void* thread ) {
        if ( thread != nullptr ) {
            ( ( Thread* ) thread )->run();
        }
    }

protected:
    Thread ();
    virtual void run () {};
private:
    thread_t myHandle;
    void (*body)(void*); void* arg;
};

class Semaphore {
public:
    Semaphore (unsigned init = 1);
    virtual ~Semaphore ();
    int wait ();
    int signal ();
    int timedWait (time_t);
    int tryWait();
private:
    sem_t myHandle;
};

class Console {
public:
    static char getc ();
    static void putc (char);
};

#endif
