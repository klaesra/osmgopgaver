#ifndef BUENOS_KERNEL_LOCK_H
#define BUENOS_KERNEL_LOCK_H

#include "kernel/spinlock.h"
#include "kernel/thread.h"

typedef int cond_t;

typedef struct {
    spinlock_t slock;
    int value;
    TID_t locked;
} lock_t;

#define LOCK_COND_MAX_LOCKS 32


int lock_reset(lock_t *lock);
void lock_acquire(lock_t *lock);
void lock_release(lock_t *lock);

void condition_init (cond_t *cond);
void condition_wait (cond_t *cond, lock_t *lock );
void condition_signal (cond_t *cond, lock_t *lock );
void condition_broadcast (cond_t *cond, lock_t *lock );


#endif /* BUENOS_KERNEL_LOCK_H */
