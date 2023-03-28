#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
//#include "zemaphore.h"
#include <semaphore.h>
#include "msgq.h"

// SEE Labs/GdbLldbLab for more information on lldb - lowlevel debugger

struct msgq *mq;

//
// Main threads
//
char *messages[] = { "msg1", "msg2", "hellomsg", "gustymsg" };
//char *messages2[] = {"msg1","msg2","msg3","msg4","msg5","msg6","msg7","msg8","msg9","msg10","msg11","msg12","msg13","msg14","msg15","msg16","msg17","msg18","msg19","msg20","msg21","msg22","msg23","msg24","msg25","msg26","msg27","msg28","msg29","msg30","msg31","msg32","msg33","msg34","msg35","msg36","msg37","msg38","msg39","msg40","msg41","msg42","msg43","msg44","msg45","msg46","msg47","msg48","msg49","msg50","msg51","msg52","msg53","msg54","msg55","msg56","msg57","msg58","msg59","msg60","msg61","msg62","msg63","msg64","msg65","msg66","msg67","msg68","msg69","msg70","msg71","msg72","msg73","msg74","msg75","msg76","msg77","msg78","msg79","msg80","msg81","msg82","msg83","msg84","msg85","msg86","msg87","msg88","msg89","msg90","msg91","msg92","msg93","msg94","msg95","msg96","msg97","msg98","msg99","msg100"};

char *messages2[] = {"msg1","msg2","msg3","msg4","msg5","msg6","msg7","msg8","msg9","msg10","msg11","msg12","msg13","msg14","msg15","msg16","msg17","msg18","msg19","msg20","msg21","msg22","msg23","msg24","msg25","msg26","msg27","msg28","msg29","msg30","msg31","msg32","msg33","msg34","msg35","msg36","msg37","msg38","msg39","msg40","msg41","msg42","msg43","msg44","msg45","msg46","msg47","msg48","msg49", "msg50"};


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

#define MSGQLEN 100

int main(int argc, char *argv[]) {
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
        mq = msgq_init(sizeof(messages2)*3);
        printf("producer comsumer\n");
        pthread_create(&p1 ,NULL,send2, NULL);
        pthread_join(p1, NULL);
        pthread_create(&p2, NULL, send2, NULL);
        pthread_join(p2, NULL);
        sleep(5);
        pthread_create(&p3, NULL, recvMsgs, NULL);
        pthread_join(p3, NULL);
        pthread_create(&p4, NULL, recvMsgs, NULL);
        pthread_join(p4, NULL);
        pthread_create(&p5, NULL, recvMsgs, NULL);
        pthread_join(p5, NULL);
        break;


      default:
        printf("invalid test selection!\n");
        break;
    }
    return 0;
}

