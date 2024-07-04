#ifndef syscall_c
#define syscall_c

#include "../lib/hw.h"
#include "MemoryAllocator.hpp"
#include "tcb.hpp"
#include "../h/sem.hpp"
#include "../lib/console.h"

using Body = void (*)(void*);

typedef TCB* thread_t;
typedef sem* sem_t;

void *mem_alloc( size_t size );

int mem_free( void* chunk );

int thread_create( thread_t* handle, void ( *start_routine ) ( void* ), void* arg );

int thread_exit( );

void thread_dispatch();

void thread_start( TCB* );

int thread_no_start ( thread_t* handle, void ( *start_routine ) ( void* ), void* arg );

int sem_open( sem_t* handle, unsigned init );

int sem_close( sem_t handle );

int sem_wait( sem_t id );

int sem_signal( sem_t id );

int sem_trywait( sem_t id );

int time_sleep( time_t );

char getc();

void putc( char c );

#endif