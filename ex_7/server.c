#define _POSIX_C_SOURCE 200809L
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

#include "message.h"
 
#define MAX_CLIENTS 10

int loop = 1;

void handle_sig(){
    loop = 0;
}

 int main(int argc, char* argv[]){
    signal(SIGINT, handle_sig);

    struct mq_attr attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = sizeof(struct Message),
        .mq_curmsgs = 0
    };

    mqd_t i_queue = mq_open("/chatting", O_RDONLY | O_CREAT, 0666, &attr);
    printf("Server /chatting started\n");

    mqd_t client_queues[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++){
        client_queues[i] = -1;
    }

    int id = 0;
    struct Message msg;

    while (loop) {
        ssize_t status = mq_receive(i_queue, (char*)&msg, sizeof(struct Message), NULL);
        if (status == -1){
            if (errno == EINTR){
                continue;
            }
            else {
                break;
            }
        }

        if (strcmp("INIT", msg.message) == 0) {
            while (client_queues[id] != -1 && id < MAX_CLIENTS)
                id++;

            if (id == MAX_CLIENTS) printf("Server is full\n");

            client_queues[id] = mq_open(msg.queue_name, O_WRONLY);

            struct Message output;
            output.number = id;

            mq_send(client_queues[id], (char*)&output, sizeof(struct Message), 10);

            printf("Client %d connected, queue name: %s\n", id, msg.queue_name);

            id = 0;
        }

        else if (strcmp("CLOSE", msg.message) == 0) {
            mq_close(client_queues[msg.number]);
            client_queues[msg.number] = -1;
            printf("Client %d disconnected\n", msg.number);
        }

        else {
            for (int i = 0; i < MAX_CLIENTS; i++){
                if (i != msg.number && client_queues[i] != -1){
                    struct Message output;
                    output.number = msg.number;
                    strcpy(output.message, msg.message);
                    mq_send(client_queues[i], (char*)&output, sizeof(struct Message), 10);
                }
            }
        }
        
    }
    mq_close(i_queue);
    mq_unlink("/chatting");
    return 0;
}