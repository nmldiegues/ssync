//array based lock
//basically identical to clh lock, but requires a bit more space
#ifndef _ALOCK_H_
#define _ALOCK_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#ifndef __sparc__
#include <numa.h>
#endif
#include <pthread.h>
#include "utils.h"
#include "atomic_ops.h"

#ifdef __sparc__
#define MAX_NUM_PROCESSES 64
#elif defined(__tile__)
#define MAX_NUM_PROCESSES 36
#elif defined(OPTERON)
#define MAX_NUM_PROCESSES 48
#else
#define MAX_NUM_PROCESSES 80
#endif

typedef struct flag_line {
    volatile uint16_t flag;
#ifdef ADD_PADDING
    uint8_t padding[CACHE_LINE_SIZE-2];
#endif
} flag_t;

typedef struct lock_shared {
    volatile uint32_t tail;
    uint32_t size;
    flag_t flags[MAX_NUM_PROCESSES]; 
} lock_shared_t;

typedef struct lock {
    uint32_t my_index;
    lock_shared_t* shared_data;
} array_lock_t;


//lock array initalization and desctruction
lock_shared_t** init_alock_array_global(uint32_t num_locks, uint32_t num_processes);

array_lock_t** init_alock_array_local(uint32_t thread_num, uint32_t num_locks, lock_shared_t** the_locks);

void end_alock_array_local(array_lock_t** local_locks, uint32_t size);

void end_alock_array_global(lock_shared_t** the_locks, uint32_t size);

//single lock initalization and desctruction
lock_shared_t* init_alock_global(uint32_t num_processes);

array_lock_t* init_alock_local(uint32_t thread_num, lock_shared_t* the_lock);

void end_alock_local(array_lock_t* local_lock);

void end_alock_global(lock_shared_t* the_lock);

//lock 
void alock_lock(array_lock_t* lock);

//unlock
void alock_unlock(array_lock_t* lock);

int alock_trylock(array_lock_t* local_lock);
int is_free_alock(lock_shared_t* the_lock);

#endif