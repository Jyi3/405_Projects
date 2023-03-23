#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msgq.h"


/* Returns a message from mq. The returned message is on the heap. The function
   that receives the message can free it when it is no longer needed. */
   
char *msgq_recv(struct msgq *mq) 
{
    /* Check if the message queue is valid */
    if (!mq) 
    {
        return NULL;
    }

    /* Acquire the lock to access the message queue */
    sem_wait(&mq->lock);

    /* Wait until there is at least one message in the queue */
    while (mq->num_msgs == 0) 
    {
        sem_post(&mq->lock);    /* Release the lock */
        sem_wait(&mq->has_msgs); /* Wait for a message to be added */
        sem_wait(&mq->lock);    /* Acquire the lock again */
    }

    /* Get the message from the head of the queue */
    msg_node *node = mq->head;
    char *msg = node->msg;

    /* Update the head of the queue */
    mq->head = node->next;
    mq->num_msgs--;

    /* If the queue is now empty, update the tail as well */
    if (mq->num_msgs == 0) 
    {
        mq->tail = NULL;
    }

    /* Free the message node */
    free(node);

    /* Release the lock */
    sem_post(&mq->lock);

    /* Allocate memory for the returned message on the heap */
    char *heap_msg = malloc(strlen(msg) + 1);

    /* Copy the message to the heap-allocated memory */
    strcpy(heap_msg, msg);

    /* Return the heap-allocated message */
    return heap_msg;
}
