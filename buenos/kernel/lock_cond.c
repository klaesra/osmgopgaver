#include "kernel/lock_cond.h"
#include "kernel/interrupt.h"
#include "kernel/sleepq.h"
#include "lib/libc.h"

static lock_t lock_table[LOCK_COND_MAX_LOCKS];

int lock_reset( lock_t *lock ){
    spinlock_reset(&lock->slock);
    lock->locked = -1;
    return 0;
}


void lock_acquire( lock_t *lock ) {
    interrupt_status_t intr_status;

    intr_status = _interrupt_disable();
    spinlock_acquire(&lock->slock);

    while (lock->locked >= 0) {
        sleepq_add(lock);
        spinlock_release(&lock->slock);
        thread_switch();
    }
    lock->locked = 0;
    
    spinlock_release(&lock->slock);
    _interrupt_set_state(intr_status);
    
}

void lock_release( lock_t *lock ) {
    interrupt_status_t intr_status;
    
    intr_status = _interrupt_disable();
    spinlock_acquire(&lock->slock);

    if (lock->locked < 0) {
        sleepq_wake(lock);
    }

    spinlock_release(&lock->slock);
    _interrupt_set_state(intr_status);
}


void condition_init (cond_t *cond) {
    int i;
    if (cond) {
        for (i = 0; i < LOCK_COND_MAX_LOCKS ; i++) {
            lock_reset(&lock_table[i]);
        }
    }
}

void condition_wait (cond_t *cond, lock_t *lock ) {
    if (cond) {
        lock_acquire(lock);
    }
}

void condition_signal (cond_t *cond, lock_t *lock ) {
    if (cond) {
        lock_release(lock);
    }
}

void condition_broadcast (cond_t *cond, lock_t *lock ) {
    if (cond) {
        sleepq_wake_all(lock);
    }
}
