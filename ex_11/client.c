#define _POSIX_C_SOURCE 200112L
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include "util.h"


int fd;
char name[MAX_NAME];
pthread_t receive_thread, sending_thread;

void handler(){
    printf("Closing connection\n");

    struct message msg;
    msg.type = STOP;
    send(fd, &msg, sizeof(msg), 0);

    close(fd);
    pthread_cancel(receive_thread);
    pthread_cancel(sending_thread);

    exit(0);
}

const char* time_func(time_t input_time) {
    static char buffer[20];

    struct tm *local = localtime(&input_time);
    if (local == NULL) {
        return "Error converting time";
    }

    snprintf(buffer, sizeof(buffer), "%02d-%02d-%04d %02d:%02d:%02d",
             local->tm_mday, local->tm_mon + 1, local->tm_year + 1900,
             local->tm_hour, local->tm_min, local->tm_sec);

    return buffer;
}

void *receive_messages(void* arg){
    struct message msg;
    
    while (1) {
        recv(fd, &msg, sizeof(msg), 0);
        switch (msg.type){
            case LIST:
                printf("List of clients:\n");
                for (int i = 0; i < msg.no_of_clients; i++)
                    printf("%s\n", msg.list[i]);
                break;

            case MESSAGE:
                printf("(%s) Message from %s: %s\n", time_func(msg.timestamp), msg.id, msg.message);
                break;

            case ALIVE:
                msg.type = ALIVE;
                send(fd, &msg, sizeof(msg), 0);
                break;

            case ERROR:
                printf("%s\n", msg.message);
                break;

            case STOP:
                printf("Server stopped\n");
                handler();
                break;
                
            default:
                break;
        }
    }
    return NULL;
}

MessageType get_message_type_from_string(const char *type_str) {
    if (strcmp(type_str, "LIST") == 0) return LIST;
    if (strcmp(type_str, "TO_ALL") == 0) return TO_ALL;
    if (strcmp(type_str, "TO_ONE") == 0) return TO_ONE;
    if (strcmp(type_str, "STOP") == 0) return STOP;
    return ERROR;
}

void *send_messages(void* arg) {
    char input[1024];
    char m_type[10];
    char user_id[10];

    while (1) {
        if (fgets(input, 1024, stdin) != NULL) {
            input[strcspn(input, "\n")] = 0;

            char *word = strtok(input, " ");
            if (word != NULL) strcpy(m_type, word);

            MessageType mess_type = get_message_type_from_string(m_type);
            
            if (mess_type == ERROR) {
                printf("Unknown message type\n");
                continue;
            }

            struct message msg;

            switch (mess_type) {
                case LIST:
                    msg.type = LIST;
                    send(fd, &msg, sizeof(msg), 0);
                    break;

                case TO_ALL:
                    word = strtok(NULL, "");
                    if (word == NULL) printf("No message\n");
                    else 
                    {
                        msg.timestamp = time(NULL);
                        msg.type = TO_ALL;
                        strcpy(msg.message, word);
                        send(fd, &msg, sizeof(msg), 0);
                    }
                    break;

                case TO_ONE:
                    word = strtok(NULL, " ");
                    if (word == NULL) printf("No user id\n");
                    else 
                    {
                        strcpy(user_id, word);
                        word = strtok(NULL, "");

                        if (word == NULL) printf("No message\n");
                        else 
                        {
                            msg.timestamp = time(NULL);
                            msg.type = TO_ONE;
                            strcpy(msg.message, word);
                            strcpy(msg.id,  user_id);
                            send(fd, &msg, sizeof(msg), 0);
                        }
                    }
                    break;

                case STOP:
                    handler();
                    break;

                default:
                    break;
            }
        }
    }
    return NULL;
}

int main(int argc, char *argv[]){
    char ip[MAX_NAME];
    int port = atoi(argv[3]);

    signal(SIGINT, handler);

    strncpy(name, argv[1], MAX_NAME);
    strncpy(ip, argv[2], MAX_NAME);

    fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1){
        printf("Connection failed\n");
        exit(1);
    }
    else {
        send(fd, &name, strlen(name), 0);
    }

    pthread_create(&receive_thread, NULL, receive_messages, &fd);

    pthread_create(&sending_thread, NULL, send_messages, &fd);

    pthread_join(receive_thread, NULL);

    pthread_join(sending_thread, NULL);

    return 0;
 }