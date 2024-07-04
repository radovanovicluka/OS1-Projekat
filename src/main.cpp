//
// Created by os on 5/10/24.
//

#include "../h/syscall_c.h"

extern void userMain();

extern "C" void supervisorTrap();

void wrapperMain( void *arg ) {
    userMain();
}

int main() {
    TCB *threads[5];

    MemoryAllocator::initMemory();

    __asm__ volatile ( "csrw stvec, %0" : : "r"( &supervisorTrap ) );
    __asm__ volatile ( "csrs sstatus, 0x2" );

    thread_create(&threads[0], nullptr, nullptr );
    TCB::running = threads[0];

    thread_create( &threads[1], &wrapperMain, nullptr );

    while( !threads[1]->isFinished() ) {
        thread_dispatch();
    }

    return 0;
}