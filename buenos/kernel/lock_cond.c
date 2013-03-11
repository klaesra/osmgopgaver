#include "kernel/lock_cond.h"
#include "kernel/interrupt.h"
#include "kernel/sleepq.h"
#include "lib/libc.h"

//static lock_t lock_table[LOCK_COND_MAX_LOCKS];

int lock_reset( lock_t *lock ){
    spinlock_reset(&lock->slock);
    lock->locked = LOCK_UNLOCKED;
    return 0;
}


void lock_acquire( lock_t *lock ) {
    interrupt_status_t intr_status;

    intr_status = _interrupt_disable();
    spinlock_acquire(&lock->slock);

    while (lock->locked == LOCK_LOCKED) {
        sleepq_add(lock);
        spinlock_release(&lock->slock);
        thread_switch();
    }
    lock->locked = LOCK_LOCKED;
    
    spinlock_release(&lock->slock);
    _interrupt_set_state(intr_status);
    
}

void lock_release( lock_t *lock ) {
    interrupt_status_t intr_status;
    
    intr_status = _interrupt_disable();
    spinlock_acquire(&lock->slock);

    lock->locked = LOCK_UNLOCKED;
    sleepq_wake(lock);
    
    spinlock_release(&lock->slock);
    _interrupt_set_state(intr_status);
}


void condition_init (cond_t *cond) {
    cond = cond;
}

void condition_wait (cond_t *cond, lock_t *lock ) {
    lock_release(lock);
    sleepq_add(cond);
    thread_switch();
    lock_acquire(lock);
}

void condition_signal (cond_t *cond, lock_t *lock ) {
    lock = lock;
    sleepq_wake(cond);    
}

void condition_broadcast (cond_t *cond, lock_t *lock ) {
    lock = lock;    
    sleepq_wake_all(&cond);
}
