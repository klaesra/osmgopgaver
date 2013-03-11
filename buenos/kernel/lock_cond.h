#ifndef BUENOS_KERNEL_LOCK_H
#define BUENOS_KERNEL_LOCK_H

#include "kernel/spinlock.h"
#include "kernel/thread.h"

#define LOCK_COND_MAX_LOCKS 32
#define LOCK_UNLOCKED -1
#define LOCK_LOCKED 0

typedef void* cond_t;

typedef struct {
    spinlock_t slock;
    TID_t locked;
} lock_t;

int lock_reset(lock_t *lock);
void lock_acquire(lock_t *lock);
void lock_release(lock_t *lock);

void condition_init (cond_t *cond);
void condition_wait (cond_t *cond, lock_t *lock );
void condition_signal (cond_t *cond, lock_t *lock );
void condition_broadcast (cond_t *cond, lock_t *lock );


#endif /* BUENOS_KERNEL_LOCK_H */
