#include "alloc.h"
#include <unistd.h>  // For sbrk()
#include <string.h>  // For memset, memcpy
#include <stdio.h>   // For debugging
#include <pthread.h> // For thread safety

static free_block *HEAD = NULL;  // Free list head
static free_block *last_alloc = NULL; // For Next Fit
allocation_strategy current_strategy = FIRST_FIT; // Default strategy
pthread_mutex_t allocator_mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex for thread safety

void set_allocation_strategy(allocation_strategy strategy) {
    current_strategy = strategy;
}

// Requests memory from OS
void *do_alloc(size_t size) {
    free_block *block = sbrk(size + sizeof(free_block));
    if (block == (void *) -1) return NULL;  

    block->size = size;
    block->next = NULL;
    block->magic = MAGIC;
    return (void *)(block + 1);
}

// Custom malloc() implementation with different strategies
void *tumalloc(size_t size) {
    if (size <= 0) return NULL;

    pthread_mutex_lock(&allocator_mutex);  // Lock for thread safety

    free_block *prev = NULL, *current = HEAD;
    free_block *best_prev = NULL, *worst_prev = NULL, *next_prev = NULL;
    free_block *best_block = NULL, *worst_block = NULL, *next_block = NULL;

    if (current_strategy == FIRST_FIT) {
        while (current) {
            if (current->size >= size) {
                if (prev) prev->next = current->next;
                else HEAD = current->next;
                pthread_mutex_unlock(&allocator_mutex);
                return (void *)(current + 1);
            }
            prev = current;
            current = current->next;
        }
    }
    
    if (current_strategy == BEST_FIT) {
        while (current) {
            if (current->size >= size && (!best_block || current->size < best_block->size)) {
                best_block = current;
                best_prev = prev;
            }
            prev = current;
            current = current->next;
        }
        if (best_block) {
            if (best_prev) best_prev->next = best_block->next;
            else HEAD = best_block->next;
            pthread_mutex_unlock(&allocator_mutex);
            return (void *)(best_block + 1);
        }
    }

    if (current_strategy == WORST_FIT) {
        while (current) {
            if (current->size >= size && (!worst_block || current->size > worst_block->size)) {
                worst_block = current;
                worst_prev = prev;
            }
            prev = current;
            current = current->next;
        }
        if (worst_block) {
            if (worst_prev) worst_prev->next = worst_block->next;
            else HEAD = worst_block->next;
            pthread_mutex_unlock(&allocator_mutex);
            return (void *)(worst_block + 1);
        }
    }

    if (current_strategy == NEXT_FIT) {
        if (!last_alloc) last_alloc = HEAD;
        prev = NULL;
        current = last_alloc;

        while (current) {
            if (current->size >= size) {
                last_alloc = current->next;
                if (prev) prev->next = current->next;
                else HEAD = current->next;
                pthread_mutex_unlock(&allocator_mutex);
                return (void *)(current + 1);
            }
            prev = current;
            current = current->next;
        }

        last_alloc = HEAD;
    }

    pthread_mutex_unlock(&allocator_mutex);
    return do_alloc(size);  
}

// Custom free() implementation
void tufree(void *ptr) {
    if (!ptr) return;

    pthread_mutex_lock(&allocator_mutex);

    free_block *block = (free_block *)ptr - 1;
    if (block->magic != MAGIC) {
        printf("MEMORY CORRUPTION DETECTED\n");
        pthread_mutex_unlock(&allocator_mutex);
        return;
    }

    block->next = HEAD;
    HEAD = block;

    pthread_mutex_unlock(&allocator_mutex);
}
