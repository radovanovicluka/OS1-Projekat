#include "../h/syscall_c.h"
#include "../test/printing.hpp"

//extern "C" void supervisorTrap();

extern "C" void handleSupervisorTrap() {
    uint64 scause;
    __asm__ volatile ( "csrr %0, scause" : "=r"( scause ) );

    if ( scause == 0x08UL || scause == 0x09UL) {
        uint64 volatile sepc, sstatus;
        __asm__ volatile ( "csrr %0, sepc" : "=r"( sepc ) );
        sepc += 4;

        __asm__ volatile ( "csrr %0, sstatus" : "=r"( sstatus ) );

        uint64 code, ret;
        __asm__ volatile ( "mv %0, a0" : "=r"( code ) );

        switch( code ) {
            // mem_alloc
            case 0x01: {
                uint64 blocks;
                __asm__ volatile ( "mv %0, a1" : "=r"( blocks ) );
                size_t bytes = blocks * MEM_BLOCK_SIZE;

                void *p = MemoryAllocator::mem_alloc(bytes);

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( p ) );

                break;
            }
            // mem_free
            case 0x02: {
                void *ptr;
                __asm__ volatile ( "mv %0, a1" : "=r"( ptr ) );

                int val = MemoryAllocator::mem_free( ptr );

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( val ) );

                break;
            }
            // thread_create
            case 0x11: {
                void* args, *stack;
                Body body;
                thread_t* thread;

                __asm__ volatile ( "mv %0, a1" : "=r"( thread ) );
                __asm__ volatile ( "mv %0, a2" : "=r"( body ) );

                __asm__ volatile ( "mv %0, a4" : "=r"( stack ) );
                __asm__ volatile ( "mv %0, a7" : "=r"( args ) );

                ret = TCB::createThread( thread, body, args, stack );

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( ret ) );

                break;
            }
            // thread_exit
            case 0x12: {
                ret = 0;

                TCB::running->exit();

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( ret ) );

                break;
            }
            // thread_dispatch
            case 0x13: {
                TCB::dispatch();
                break;
            }
            // thread_start
            case 0x14: {
                TCB* thread;

                __asm__ volatile ( "mv %0, a1" : "=r"(thread) );

                TCB::start( thread );
            }
            // thread_no_start
            case 0x15: {
                void* args, *stack;
                Body body;
                thread_t* thread;

                __asm__ volatile ( "mv %0, a1" : "=r"( thread ) );
                __asm__ volatile ( "mv %0, a2" : "=r"( body ) );

                __asm__ volatile ( "mv %0, a4" : "=r"( stack ) );
                __asm__ volatile ( "mv %0, a7" : "=r"( args ) );

                ret = TCB::createNoStart( thread, body, args, stack );

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( ret ) );

                break;

            }
            // sem_open
            case 0x21: {
                sem_t* handle;
                uint64 init;

                __asm__ volatile ( "mv %0, a1" : "=r"( handle ) );
                __asm__ volatile ( "mv %0, a2" : "=r"( init ) );

                ret = sem::sem_open( handle, init );

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( ret ) );

                break;
            }
            // sem_close
            case 0x22: {
                sem_t handle;

                __asm__ volatile ( "mv %0, a1" : "=r"( handle ) );

                ret = handle->close();

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( ret ) );

                break;
            }
            // sem_wait
            case 0x23: {
                sem_t id;

                __asm__ volatile ( "mv %0, a1" : "=r"( id ) );

                ret = id->wait();

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( ret ) );

                break;
            }
            // sem_signal
            case 0x24: {
                sem_t id;

                __asm__ volatile ( "mv %0, a1" : "=r"( id ) );

                ret = id->signal();

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( ret ) );

                break;
            }
            // sem_trywait
            case 0x26: {
                sem_t id;

                __asm__ volatile ( "mv %0, a1" : "=r"( id ) );

                ret = id->trywait();

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( ret ) );

                break;
            }
            // getc
            case 0x41: {
                ret = __getc();

                __asm__ volatile ( "csrw sscratch, %0" : : "r"( ret ) );

                break;
            }
            // putc
            case 0x42: {
                char c;

                __asm__ volatile ( "mv %0, a1" : "=r"( c ) );

                __putc( c );

                break;
            }

            default: {
                break;
            }
        }

        __asm__ volatile ( "csrw sstatus, %0" : : "r"( sstatus ) );
        __asm__ volatile ( "csrw sepc, %0" : : "r"( sepc ) );
    }
    else if ( scause == 0x8000000000000001UL ) {
        __asm__ volatile ( "csrc sip, 0x2" );
    }
    else if ( scause == 0x8000000000000009UL ) {
        console_handler();
    }
    else {
        printString("ERROR\n");
        printString("scause: ");
        printInt(scause);
        printString("\n");
    }
}


void *mem_alloc( size_t size ) {
    uint64 blocks = size / MEM_BLOCK_SIZE;
    void *adress;

    //Zaokruzivanje na velicinu bloka
    if( size % MEM_BLOCK_SIZE != 0) {
        blocks = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    }

    __asm__ volatile ( "mv a1, %0" : : "r" ( blocks ) );
    __asm__ volatile ( "li a0, 0x01" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( adress ) );
    return adress;
}

int mem_free( void* pointer ) {
    uint64 ret;

    __asm__ volatile ( "mv a1, %0" : : "r"( pointer ) );
    __asm__ volatile ( "li a0, 0x02" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( ret ) );

    return ret;
}
//TCB**
int thread_create( thread_t* handle, void ( *start_routine ) ( void* ), void* arg ) {
    uint64 ret;
    void* stack = ( start_routine != nullptr ? mem_alloc( DEFAULT_STACK_SIZE ) : nullptr );

    __asm__ volatile ( "mv a7, %0" : : "r"( arg ) );
    __asm__ volatile ( "mv a4, %0" : : "r"( stack ) );
    __asm__ volatile ( "mv a2, %0" : : "r"( start_routine ) );
    __asm__ volatile ( "mv a1, %0" : : "r"( handle ) );
    __asm__ volatile ( "li a0, 0x11" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( ret ) );

    return ret;
}

int thread_no_start ( thread_t* handle, void ( *start_routine ) ( void* ), void* arg ) {
    uint64 ret;
    void* stack = ( start_routine != nullptr ? mem_alloc( DEFAULT_STACK_SIZE ) : nullptr );

    __asm__ volatile ( "mv a7, %0" : : "r"( arg ) );
    __asm__ volatile ( "mv a4, %0" : : "r"( stack ) );
    __asm__ volatile ( "mv a2, %0" : : "r"( start_routine ) );
    __asm__ volatile ( "mv a1, %0" : : "r"( handle ) );
    __asm__ volatile ( "li a0, 0x11" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( ret ) );

    return ret;
}

int thread_exit( ) {
    uint64 ret;

    __asm__ volatile ( "li a0, 0x12" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( ret ) );

    return ret;
}

void thread_dispatch() {
    __asm__ volatile ( "li a0, 0x13" );

    __asm__ volatile ( "ecall" );
}

void thread_start( TCB* thread ) {
    __asm__ volatile ( "mv a1, %0" : : "r"(thread) );
    __asm__ volatile ( "li a0, 0x13" );

    __asm__ volatile ( "ecall" );
}

int sem_open( sem_t* handle, unsigned init ) {
    uint64 ret;

    __asm__ volatile ( "mv a2, %0" : : "r"( init ) );
    __asm__ volatile ( "mv a1, %0" : : "r"( handle ) );
    __asm__ volatile ( "li a0, 0x21" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( ret ) );

    return ret;
}

int sem_close( sem_t handle ) {
    uint64 ret;

    __asm__ volatile ( "mv a1, %0" : : "r"( handle ) );
    __asm__ volatile ( "li a0, 0x22" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( ret ) );

    return ret;
}

int sem_wait( sem_t id ) {
    uint64 ret;

    __asm__ volatile ( "mv a1, %0" : : "r"( id ) );
    __asm__ volatile ( "li a0, 0x23" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( ret ) );

    return ret;
}

int sem_signal( sem_t id ) {
    uint64 ret;

    __asm__ volatile ( "mv a1, %0" : : "r"( id ) );
    __asm__ volatile ( "li a0, 0x24" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( ret ) );

    return ret;
}

int sem_trywait ( sem_t id ) {
    uint64 ret;

    __asm__ volatile ( "mv a1, %0" : : "r"( id ) );
    __asm__ volatile ( "li a0, 0x26" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( ret ) );

    return ret;
}

char getc() {
    char c;
    __asm__ volatile ( "li a0, 0x41" );

    __asm__ volatile ( "ecall" );

    __asm__ volatile ( "mv %0, a0" : "=r"( c ) );

    return c;
}

void putc( char c ) {
    __asm__ volatile ( "mv a1, %0" : : "r"( c ) );
    __asm__ volatile ( "li a0, 0x42" );

    __asm__ volatile ( "ecall" );
}

int time_sleep( time_t time ) {
    return 0;
}
