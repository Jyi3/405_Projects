#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "zemaphore.h"
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
    int total_msgs = 0;
    msg_node *head;
    msg_node *tail;
} msgq;


struct msgq *msgq_init(int num_msgs){
    msgq *mq = malloc(sizeof(msgq));
    mq->num_msgs = num_msgs;
    mq->head = NULL;
    mq->tail = NULL;
    return mq; 
}

int msgq_send(struct msgq *mq, char *msg){
    if (!mq) {
        return -1;
    }
    if (mq == NULL || msg == NULL){
        return -1;
    }

    char *copy = malloc(strlen(msg) + 1);
    if (copy == NULL) {
        return -1;
    }
    strcpy(copy, msg);
    while (mq->total_msgs == mq->num_msgs){
        //do nothing(block) if the msgq is full
    }
    sem_wait(&mq->lock);   


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
    return 1;

}

char *msgq_recv(struct msgq *mq);
