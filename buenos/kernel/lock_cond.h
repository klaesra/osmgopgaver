#ifndef BUENOS_KERNEL_LOCK_H
#define BUENOS_KERNEL_LOCK_H

typedef int lock_t;

int lock_reset(lock_t *lock);
void lock_acquire(lock_t *lock);
void lock_release(lock_t *lock);

void condition_init (cond t *cond);
void condition_wait (cond t *cond, lock t *lock );
void condition_signal (cond t *cond, lock t *lock );
void condition_broadcast (cond t *cond, lock t *lock );


#endif /* BUENOS_KERNEL_LOCK_H */
