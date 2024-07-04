
#include "../h/tcb.hpp"
#include "../h/syscall_c.h"

extern "C" void saveRegisters();
extern "C" void loadRegisters();

TCB* TCB::running = nullptr;

void TCB::setFinished(bool finished) {
    this->finished = finished;
}

TCB::TCB( Body body, void *args, void *stack ) : body(body), stack(
        body != nullptr ? new uint64[DEFAULT_STACK_SIZE] : nullptr
        ), context(
        {((uint64) &TCB::wrapper),
         body == nullptr ? 0 : (uint64) &this->stack[ DEFAULT_STACK_SIZE ]
        }), args( args ), finished(false), blocked(false) {}

void TCB::wrapper() {
    popSppSpie();
    running->body( (void*)running->args );
    running->setFinished( true );
    thread_dispatch();
}

void TCB::dispatch() {
//    save registers
//    saveRegisters();

    TCB* old = running;
    if ( !old->isFinished() && !old->isBlocked() ) {
        Scheduler::put( old );
    }

    running = Scheduler::get();

    if ( old != running ) {
        TCB::yield(&old->context, &running->context);
    }

//    load registers
//    loadRegisters();
}

void TCB::exit() {
    running->setFinished( true );
    dispatch();
}

void TCB::popSppSpie() {
    __asm__ volatile ( "csrw sepc, ra" );
    __asm__ volatile ( "sret" );
}



