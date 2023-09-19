#include<mutex>
#include<semaphore>
class RwLock {
public:
    RwLock();
    int rwlock_init();
    int rwlock_destroy();
    int rwlock_lock_r();
    int rwlock_lock_w();
    int rwlock_unlock();
};
