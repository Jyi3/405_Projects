#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "msgq.h"

#ifdef _WIN32
    #include <semaphore.h>
#elif __APPLE__
    #include "zemaphore.h"
#endif

#define MSGQLEN 100
#define ARRAYLEN 100
#define DEFMESSAGES 50
#define DEFPRODUCERS 2
#define DEFCONSUMERS 3

struct msgq *mq;

//
// Main threads
//
char *messages[] = { "msg1", "msg2", "hellomsg", "gustymsg" };


void *messageMaker(void *arg) 
{
    int producer_id = *((int *) arg);
    for (int i = 0; i < DEFMESSAGES; i++) {
        char *message = ;
        printf("Producer %d sending: %s\n", producer_id, message);
        msgq_send(mq, message);
    }
    return NULL;
}

// sends msgs in messages[]
void *promtAndSend(void *arg) {
    for (int i = 0; i < sizeof(messages)/sizeof(char*); i++) {
        char response[80];
        printf("Send? ");
        scanf("%s", response);
        if (response[0] == 'y' || response[0] == 'Y') {
            printf("sending: %s\n", messages[i]);
            msgq_send(mq, messages[i]);
            int length  = msgq_len(mq);
            printf("%d\n", length);
        }
    }
    return NULL;
}
// sends msgs in messages2[]
void *send2(void *arg) {
    for (int i = 0; i < sizeof(messages2)/sizeof(messages2[0]); i++) {
            printf("sending: %s\n", messages2[i]);
            msgq_send(mq, messages2[i]);
//            int length  = msgq_len(mq);
//            printf("%d\n", length);
        }
    return NULL;
}


// consume messges in msgq
void *recvMsgs(void *arg) {
    sleep(5);
    int msg_count = msgq_len(mq);
    printf("mq msg_count: %d\n", msg_count);
    for (int i = 0; i < msg_count; i++) {
        char *m = msgq_recv(mq);
        printf("recvMsgs: %s\n", m);
        //free(m);
    }
    return NULL;
}
//recieves msg from mq. writes to an array
void *recvAndWrite(void *arg){
    char** array = (char**)arg;// cast array from void to char
    int count = 0;
    int msg_count = msgq_len(mq);
    for (int i = 0; i < msg_count; i++){
        char *m = msgq_recv(mq);
        array[count] = malloc(strlen(m) + 1);
        strcpy(array[count], m);
//        printf("test %s\n", array[count]);
        count++;
    }
    return NULL;    
}


void *passiton(void *arg) {
    int* me_ptr = (int*) arg;
    int me = *me_ptr;
    while (1) {
        sleep(1);
        printf("passiton%d initial msgq_len: %d\n", me, msgq_len(mq));
        char *m = msgq_recv(mq);
        printf("passiton%d: %p %p %s\n", me, &m, m, m);
        printf("passiton%d after recv msgq_len: %d\n", me, msgq_len(mq));
        msgq_send(mq, m);
        printf("passiton%d after send msgq_len: %d\n", me, msgq_len(mq));
        free(m);
    }
    return NULL;
}


int main(int argc, char *argv[]) {
    char *c1[ARRAYLEN];//array for consumer 1
    char *c2[ARRAYLEN];//array for consumer 2
    char *c3[ARRAYLEN];//array for consumer 3
    pthread_t p1, p2, p3, p4, p5;
    mq = msgq_init(MSGQLEN);
    char test = '1';
    if (argc == 2)
        test = argv[1][0];
    switch (test) {
      case '1':
        printf("test fill and empty msgq\n");
        pthread_create(&p1, NULL, promtAndSend, NULL);
        pthread_join(p1, NULL);
        printf("msgq_show() after filling for test 1:\n");
        msgq_show(mq);
        pthread_create(&p2, NULL, recvMsgs, NULL);
        pthread_join(p2, NULL);
        printf("msgq_show() after all consumed by test 1:\n");
        msgq_show(mq);
        break;
      case '2':
        printf("test fill msgs and pass it on\n");
        pthread_create(&p1, NULL, promtAndSend, NULL);
        pthread_join(p1, NULL);
        printf("msgq_show() after filling for test 2:\n");
        msgq_show(mq);
        printf("james1\n");
        pthread_create(&p1, NULL, passiton, (void *)1);
        printf("james2\n");
        pthread_create(&p2, NULL, passiton, (void *)2);
        printf("james3\n");
        pthread_join(p1,NULL);
        printf("james4\n");
        pthread_join(p2,NULL);
        printf("james5\n");
        break;
      case '3':
        //mq = msgq_init(sizeof(messages2)*3);
        mq = msgq_init(DEFMESSAGES*DEFPRODUCERS);
        printf("producer comsumer\n");
        pthread_t producers[DEFPRODUCERS], consumers[DEFCONSUMERS];

        for (int i = 0; i < DEFPRODUCERS; i++)
        {
            pthread_create(&producers[i], NULL, messageMaker, (void) &producer_ids[i]);
        }

        for (int i = 0; i < DEFPRODUCERS; i++) 
        {
            pthread_join(producers[i], NULL);
        }

        for (int i = 0; i < DEFCONSUMERS; i++) 
        {
            pthread_create(&consumers[i], NULL, consumer, &consumer_ids[i]);
        }


        break;


      default:
        printf("invalid test selection!\n");
        break;
    }
    return 0;

}

