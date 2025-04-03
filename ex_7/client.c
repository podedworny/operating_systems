#include <mqueue.h>
#include <stdio.h> 
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"

int loop = 1;

void handle_sig(){
    loop = 0;
    printf("hadnle\n");
}

int main(int argc, char* argv[]){
    signal(SIGINT, handle_sig);

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(struct Message),
        .mq_curmsgs = 0
    };

    pid_t pid = getpid();
    char queue_name[1024];
    sprintf(queue_name, "/queue_id_%d", pid);

    mqd_t queue = mq_open(queue_name, O_RDONLY | O_CREAT, 0666, &attr);

    struct Message msg;
    strcpy(msg.message, "INIT");
    strcpy(msg.queue_name, queue_name);

    mqd_t server_queue = mq_open("/chatting", O_WRONLY);
    mq_send(server_queue, (char*)&msg, sizeof(struct Message), 10);

    int id;
    struct Message input;
    mq_receive(queue, (char*)&input, sizeof(struct Message), NULL);

    id = input.number;
    printf("Connected to server, id: %d\n", id);

    pid_t child_process = fork();

    if (child_process == 0)
    {   // receiver
        struct Message receive;
        while (loop)
            if (mq_receive(queue, (char*)&receive, sizeof(struct Message), NULL) != -1)
                printf("Message from Client %d: %s\n", receive.number, receive.message);

        exit(0);
    }
    else 
    { // sender
        char send[1024];
        while(loop)
        {
            if (fgets(send, 100, stdin) != NULL){
                send[strlen(send) - 1] = '\0';
                struct Message send_msg;
                send_msg.number = id;
                strcpy(send_msg.message, send);
                mq_send(server_queue, (char*)&send_msg, sizeof(struct Message), 10);
            }
        }
    }
    struct Message close_msg;
    strcpy(close_msg.message, "CLOSE");
    close_msg.number = id;
    mq_send(server_queue, (char*)&close_msg, sizeof(struct Message), 10);

    mq_close(queue);
    mq_close(server_queue);
    mq_unlink(queue_name);
    return 0;
}