// You must include <pthread.h> before include "zemaphore.h"
typedef struct __zem_t {
    int value;
    pthread_cond_t  cond;
    pthread_mutex_t lock;
} zem_t;


void zem_wait(zem_t *s);
void zem_post(zem_t *s);
void zem_init(zem_t *s, int v);
