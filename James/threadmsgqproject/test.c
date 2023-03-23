#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "msgq.h"

#define NUM_MSGS 10

struct msgq *mq;
sem_t has_msgs;

void *producer(void *arg) 
{
    char *msgs[] = {"Hello", "World", "This", "Is", "A", "Test", "Of", "The", "Message", "Queue"};

    for (int i = 0; i < NUM_MSGS; i++) {
        /* Allocate memory for a new message node */
        msg_node *node = malloc(sizeof(msg_node));
        node->msg = msgs[i];
        node->next = NULL;

        /* Acquire the lock to access the message queue */
        sem_wait(&mq->lock);

        /* Add the message node to the tail of the queue */
        if (mq->tail) {
            mq->tail->next = node;
        } else {
            mq->head = node;
        }
        mq->tail = node;
        mq->num_msgs++;

        /* Signal that a message has been added to the queue */
        sem_post(&mq->has_msgs);

        /* Release the lock */
        sem_post(&mq->lock);
    }

    return NULL;
}

void *consumer(void *arg) {
    for (int i = 0; i < NUM_MSGS; i++) {
        /* Receive a message from the message queue */
        char *msg = msgq_recv(mq);

        /* Print the message */
        printf("Received message: %s\n", msg);

        /* Free the heap-allocated message */
        free(msg);
    }

    return NULL;
}

int main() {
    /* Initialize the message queue */
    mq = msgq_init(10);

    /* Initialize the semaphore for signaling when a message has been added */
    sem_init(&mq->has_msgs, 0, 0);

    /* Initialize the lock for accessing the message queue */
    sem_init(&mq->lock, 0, 1);

    /* Create a producer thread */
    pthread_t prod_tid;
    pthread_create(&prod_tid, NULL, producer, NULL);

    /* Create a consumer thread */
    pthread_t cons_tid;
    pthread_create(&cons_tid, NULL, consumer, NULL);

    /* Wait for the threads to finish */
    pthread_join(prod_tid, NULL);
    pthread_join(cons_tid, NULL);

    /* Free the message queue */
    free(mq);

    return 0;
}
