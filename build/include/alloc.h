#ifndef CYB3053_PROJECT2_ALLOC_H
#define CYB3053_PROJECT2_ALLOC_H

#include <stddef.h>  
#include <pthread.h> // For thread safety

#define MAGIC 0x01234567  // Used for memory corruption checks

// Structure to represent a block in the free list
typedef struct free_block {
    size_t size;
    struct free_block *next;
    unsigned int magic;
} free_block;

// Global variable for free list
extern free_block *HEAD;

// Mutex for thread safety
extern pthread_mutex_t allocator_mutex;

// Function prototypes
void *tumalloc(size_t size);
void *tucalloc(size_t num, size_t size);
void *turealloc(void *ptr, size_t new_size);
void tufree(void *ptr);

// Allocation strategies
typedef enum { FIRST_FIT, BEST_FIT, WORST_FIT, NEXT_FIT } allocation_strategy;
void set_allocation_strategy(allocation_strategy strategy);

#endif // CYB3053_PROJECT2_ALLOC_H
