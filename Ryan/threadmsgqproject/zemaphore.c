#include <pthread.h>
#include "zemaphore.h"

void zem_init(zem_t *z, int value) {
    z->value = value;
    pthread_cond_init(&z->cond, NULL);
    pthread_mutex_init(&z->lock, NULL);
}

void zem_wait(zem_t *z) {
    pthread_mutex_lock(&z->lock);
    while (z->value <= 0)
	pthread_cond_wait(&z->cond, &z->lock);
    z->value--;
    pthread_mutex_unlock(&z->lock);
}

void zem_post(zem_t *z) {
    pthread_mutex_lock(&z->lock);
    z->value++;
    pthread_cond_signal(&z->cond);
    pthread_mutex_unlock(&z->lock);
}
