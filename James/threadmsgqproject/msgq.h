#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
//#include "zemaphore.h"
#include <string.h>
#include <semaphore.h>

//● struct msgq *msgq_init(int num_msgs); - initializes a message queue and returns
//a pointer to a struct msgq. The parameter num_msgs is the maximum number of
//messages that may be in the message queue. The returned pointer is used in the other
//API functions.
//● int msgq_send(struct msgq *mq, char *msg); - places msg on message queue mq.
//mq is returned from msgq_init. msgq_send must make a copy of msg on the heap. If mq
//has num_msgs in it; then msgq_send blocks until there is room for msg. msgq_send
//returns 1 for success and -1 for failure.
//● char *msgq_recv(struct msgq *mq); - returns a message from mq. mq is returned
//from msgq_init. The returned message is on the heap. The function that receives the
//message can free it when it is no longer needed.
//● int msgq_len(struct msgq *mq); - returns the number of messages on mq. mq is
//returned from msgq_init. Due to the nature of threads and interrupts, the length
//returned may be incorrect by the time it is used.
//1
//● void msgq_show(struct msgq *mq); - displays all of the messages in mq to stdout.
//mq is returned from msgq_init.
//You must design and implement struct msgq, which must involve dynamically allocated
//memory. My solution includes a linked list of messages, where send_msg mallocs memory for
//the data structure and places it on the end of the linked list. The data structure has a char*
//pointer to the message. The heap memory for the message is allocated using strdup.
//recv_msg removes the head from the linked list and returns the char* pointer to the message.


typedef struct msg_node{
    char *message;
    struct msg_node *next;
}msg_node;

typedef struct msgq
{
    int num_msgs;
    int total_msgs;
    msg_node *head;
    msg_node *tail;
    sem_t lock;
    sem_t has_msgs;
} msgq;


struct msgq *msgq_init(int num_msgs){
    msgq *mq = malloc(sizeof(msgq));
    mq->num_msgs = num_msgs;
    mq->total_msgs = 0;
    mq->head = NULL;
    mq->tail = NULL;
    sem_init(&mq->lock, 0, 1);
    sem_init(&mq->has_msgs, 0, 0);
    return mq; 
}

int msgq_send(struct msgq *mq, char *msg){
    if (!mq || !msg) {
        return -1;
    }

    char *copy = malloc(strlen(msg) + 1);
    if (copy == NULL) {
        return -1;
    }
    strcpy(copy, msg);

    sem_wait(&mq->lock);   

    while (mq->total_msgs == mq->num_msgs){
        sem_post(&mq->lock);    /* Release the lock */
        sem_wait(&mq->has_msgs); /* Wait for a message to be removed */
        sem_wait(&mq->lock);    /* Acquire the lock again */
    }

    msg_node *node = malloc(sizeof(msg_node));
    node->message = copy;
    node->next = NULL;
    if (mq->head == NULL){
        mq->head = node;
    }else {
        mq->tail->next = node;
    }
    mq->tail = node;
    mq->total_msgs ++;
    sem_post(&mq->lock); 
    sem_post(&mq->has_msgs); /* Signal that a message has been added */
    return 1;

}


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
    char *msg = node->message;

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

    /* Release the lock for has_msgs */
    sem_post(&mq->has_msgs);

    /* Return the heap-allocated message */
    return heap_msg;
}

int msgq_len(struct msgq *mq){
    /* Check if the message queue is valid */
    if (!mq) {
        return -1;
    }
    /* Acquire the lock to access the message queue */
    sem_wait(&mq->lock);

    int len = mq->total_msgs;

    /* Release the lock */
    sem_post(&mq->lock);

    return len;
}

void msgq_show(struct msgq *mq)
{
    if (!mq) {
        printf("Invalid message queue\n");
        return;
    }

    printf("Number of messages in the queue: %d\n", mq->num_msgs);

    msg_node *current_node = mq->head;
    int i = 1;
    while (current_node != NULL) {
        printf("Message %d: %s\n", i, current_node->message);
        current_node = current_node->next;
        i++;
    }
}
