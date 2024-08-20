#include "../h/syscall_cpp.hpp"

Thread::Thread(void (*body)(void *), void *arg) {
    thread_create(&myHandle, body, arg);
}

Thread::~Thread() {
    delete myHandle;
}

int Thread::start() {
    thread_start( this->myHandle );
    return 0;
}

void Thread::dispatch() {
    thread_dispatch();
}

int Thread::sleep(time_t t) {
    return 0;
}

Thread::Thread() {
    thread_no_start( &myHandle, &runWrapper, this );
}

Semaphore::Semaphore(unsigned int init) {
    sem_open( &this->myHandle, init );
}

int Semaphore::wait() {
    return sem_wait(myHandle);
}

int Semaphore::signal() {
    return sem_signal(myHandle);
}

int Semaphore::timedWait(time_t) {
    return 0;
}

int Semaphore::tryWait() {
    return sem_trywait(myHandle);
}

Semaphore::~Semaphore() {
    sem_close( myHandle );
}

char Console::getc() {
    return ::getc();
}

void Console::putc(char c) {
    ::putc(c);
}
