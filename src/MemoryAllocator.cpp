//
// Created by os on 5/7/24.Me
//
#include "../h/MemoryAllocator.hpp"

MemoryAllocator::DataChunk* MemoryAllocator::freeMem;

int MemoryAllocator::mem_free( void *mem ) {
    if ( mem == nullptr || (char*)mem - sizeof(DataChunk) < HEAP_START_ADDR || (char*)mem + MEM_BLOCK_SIZE > HEAP_END_ADDR ) return -1;

    DataChunk *blk = (DataChunk*) ((char*)mem - sizeof(DataChunk));

    DataChunk *tmp = MemoryAllocator::freeMem;
    // blk se nalazi pre freeMem
    if ( (char*)tmp >= (char*)blk + blk->size ) {
        blk->next = tmp;
        tmp->prev = blk;
        blk->prev = nullptr;
        MemoryAllocator::freeMem = blk;
        tryToJoin(blk);
        return 0;
    }
    // blk se nalazi posle freeMem
    else {
        for (; tmp != nullptr; tmp = tmp->next) {
            if ((char *) tmp + tmp->size <= (char*)blk && (char*)tmp->next >= (char *) blk + blk->size) {
                break;
            }
        }
        if ( tmp == nullptr ) return -2;

        blk->next = tmp->next;
        tmp->next = blk;
        blk->prev = tmp;
        blk->next->prev = blk;
        tryToJoin(tmp);
        tryToJoin(blk);
        return 0;
    }
}

void* MemoryAllocator::mem_alloc( size_t size ) {
    if ( size <= 0 || MemoryAllocator::freeMem == nullptr ) return nullptr;

    //Zaokruzivanje na velicinu bloka
    if( size % MEM_BLOCK_SIZE != 0) {
        size = ((size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE) * MEM_BLOCK_SIZE;
    }

    DataChunk* tmp = MemoryAllocator::freeMem;
    for ( ; tmp != nullptr; tmp = tmp->next ) {
        if ( tmp->size >= size + sizeof( DataChunk ) ) {
            break;
        }
    }

    if ( tmp == nullptr ) return nullptr;

    DataChunk *newChunk;
    if ( tmp->size > size + sizeof( DataChunk ) ) {
        // Fragmentira se tekuci blok
        newChunk = ( DataChunk* ) ( (char*)tmp + size + sizeof( DataChunk ) );
        newChunk->size = tmp->size - size - sizeof( DataChunk );
        tmp->size = size + sizeof(DataChunk);

        newChunk->next = tmp->next;
    }
    else {
        // Ne Fragmentira se tekuci blok
        newChunk = tmp->next;
    }

    // Prelancavanje
    if ( tmp->prev != nullptr ) {
        tmp->prev->next = newChunk;
        newChunk->prev = tmp->prev;
    }
    else {
        MemoryAllocator::freeMem = newChunk;
        newChunk->prev = nullptr;
    }

    return (void*)( (char*)tmp + sizeof(DataChunk) );
}


// Pomocna funkcija
// Spaja curr i curr->next ako su ispunjeni uslovi
void MemoryAllocator::tryToJoin( DataChunk* curr ) {
    if ( curr != nullptr && curr->next != nullptr && (char*) curr + curr->size == (char*)( curr->next ) ) {
        curr->size += curr->next->size;
        curr->next = curr->next->next;
        if ( curr->next != nullptr ) {
            curr->next->prev = curr;
        }
    }
}
