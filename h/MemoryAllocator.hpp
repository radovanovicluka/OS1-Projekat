#ifndef _MemoryAllocator_
#define _MemoryAllocator_

#include "../lib/hw.h"

class MemoryAllocator {
private:

    struct DataChunk {
        DataChunk *next, *prev;
        size_t size;
    };

    static DataChunk* freeMem;
public:
    static void* mem_alloc( size_t size );
    static int mem_free( void* mem );

    // Pomocna funkcija
    static void tryToJoin( DataChunk* cur );

    static void initMemory() {
        freeMem = (DataChunk*) HEAP_START_ADDR;
        freeMem->next = freeMem->prev = nullptr;
        freeMem->size = ((char*)HEAP_START_ADDR - (char*)HEAP_END_ADDR - sizeof(DataChunk));
    }
};

#endif