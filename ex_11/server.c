#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "util.h"


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int fd;
int current_clients = 0;
int clients_fd[MAX_CLIENTS] = {};
char clients_names[MAX_CLIENTS][MAX_NAME] = {};
bool alive[MAX_CLIENTS] = {false};
pthread_t thread;
pthread_t alive_thread;

void handler(int signum){
   pthread_cancel(thread);
   pthread_cancel(alive_thread);
   struct message msg;
   msg.type = STOP;
   for(int i = 0; i < MAX_CLIENTS; i++){
      if (clients_fd[i] != 0){
         send(clients_fd[i], &msg, sizeof(msg), 0);
         close(clients_fd[i]);
      }
   }
   close(fd);

   exit(0);
}

int find_user(char *name){
   pthread_mutex_lock(&mutex);

   for (int i = 0; i < MAX_CLIENTS; i++){
      if (strcmp(clients_names[i], name) == 0){
         pthread_mutex_unlock(&mutex);
         return i;
      }
   }

   pthread_mutex_unlock(&mutex);
   return -1;
}

void list_of_clients(char list[MAX_CLIENTS][MAX_NAME]){
   int j = 0;
   pthread_mutex_lock(&mutex);

   for (int i = 0; i < MAX_CLIENTS; ++i){
      if (clients_fd[i] != 0){
         strncpy(list[j], clients_names[i], MAX_NAME);
         j++;
      }
   }

   pthread_mutex_unlock(&mutex);
   return;
}

int find_place(){
   pthread_mutex_lock(&mutex);

   for (int i = 0; i < MAX_CLIENTS; i++){
      if (clients_fd[i] == 0){
         pthread_mutex_unlock(&mutex);
         return i;
      }
   }

   pthread_mutex_unlock(&mutex);
   return -1;
}

void *handle_client(void* arg){
   struct arr *client = (struct arr *)arg;
   int client_fd = client->fd;
   int place = client->place;

   struct message in_msg;
   struct message out_msg;
   int id;

   while (recv(client_fd, &in_msg, sizeof(in_msg), 0) > 0){
         switch (in_msg.type){
            case LIST:
                  out_msg.type = LIST;
                  out_msg.no_of_clients = current_clients;
                  list_of_clients(out_msg.list);

                  send(client_fd, &out_msg, sizeof(out_msg), 0);
                  break;

            case TO_ALL:
                  out_msg.type = MESSAGE;
                  strncpy(out_msg.message, in_msg.message, sizeof(in_msg.message));
                  strcpy(out_msg.id, clients_names[place]);
                  out_msg.timestamp = in_msg.timestamp;

                  for (int i = 0; i < MAX_CLIENTS; ++i){
                     if (clients_fd[i] != client_fd && clients_fd[i] != 0){
                        send(clients_fd[i], &out_msg, sizeof(out_msg), 0);
                     }
                  }
                  break;

            case TO_ONE:
                  id = find_user(in_msg.id);
                  if (id == -1)
                  {
                     out_msg.type = ERROR;
                     char error_msg[] = "User not found";
                     strncpy(out_msg.message, error_msg, sizeof(out_msg.message));
                     send(client_fd, &out_msg, sizeof(out_msg), 0);
                  }
                  else 
                  {
                     out_msg.type = MESSAGE;
                     strcpy(out_msg.id, clients_names[place]);
                     out_msg.timestamp = in_msg.timestamp;
                     strncpy(out_msg.message, in_msg.message, sizeof(in_msg.message));
                     send(clients_fd[id], &out_msg, sizeof(out_msg), 0);
                  }

                  break;

            case STOP:
                  if (clients_fd[place] == client_fd && clients_fd[place] != 0){
                     printf("Client %s disconnected\n", clients_names[place]);
                     close(client_fd);

                     pthread_mutex_lock(&mutex);
                     current_clients--;
                     clients_fd[place] = 0;
                     clients_names[place][0] = '\0';
                     pthread_mutex_unlock(&mutex);
                  } 
                  break;   

            case ALIVE:
                  pthread_mutex_lock(&mutex);
                  alive[place] = true;
                  pthread_mutex_unlock(&mutex);
                  break;

            default:
                  out_msg.type = ERROR;
                  char error_msg[] = "Unknown command";
                  strncpy(out_msg.message, error_msg, sizeof(out_msg.message));
                  send(client_fd, &out_msg, sizeof(out_msg), 0);
                  break;
      }
   }

   free(client);
   close(client_fd);

   return NULL;
}

void *accept_clients(void *arg) {
   int fd = *(int *)arg;

   struct sockaddr_in client_addr;
   socklen_t client_len = sizeof(client_addr);

   int f;
   char name[MAX_NAME];

   while (1) {
      f = accept(fd, (struct sockaddr*)&client_addr, &client_len);
      struct arr *a = malloc(sizeof(struct arr));
      int place = find_place();

      if (place == -1){
         close(f);
         free(a);
         continue;
      }

      memset(name, 0, MAX_NAME);
      int bytes_received = recv(f, name, MAX_NAME - 1, 0);

      if (bytes_received <= 0){
         close(f);
         free(a);
         continue;
      }

      name[bytes_received] = '\0';

      pthread_mutex_lock(&mutex);
      clients_fd[place] = f;
      current_clients++; 
      alive[place] = true;

      strncpy(clients_names[place], name, MAX_NAME - 1);
      clients_names[place][MAX_NAME - 1] = '\0';
      name[0] = '\0';

      a->place = place;
      a->fd = clients_fd[place];
      pthread_mutex_unlock(&mutex);

      pthread_create(&thread, NULL, handle_client, a);
   }

   return NULL;
}

void *ping(void *arg){
   struct message msg;
   msg.type = ALIVE;

   while (1) {
      for (int i = 0; i < MAX_CLIENTS; ++i)
      {
         pthread_mutex_lock(&mutex);
         if (clients_fd[i] != 0) 
            send(clients_fd[i], &msg, sizeof(msg), 0);
         pthread_mutex_unlock(&mutex);
      }
      sleep(1);
      for (int i = 0; i < MAX_CLIENTS; ++i){
         if (clients_fd[i] != 0 && !alive[i]){
            printf("Client %s disconnected\n", clients_names[i]);
            close(clients_fd[i]);

            pthread_mutex_lock(&mutex);
            current_clients--;
            clients_fd[i] = 0;
            clients_names[i][0] = '\0';
            pthread_mutex_unlock(&mutex);
         }
         else{
            pthread_mutex_lock(&mutex);
            alive[i] = false;
            pthread_mutex_unlock(&mutex);

         }
      }
      sleep(5);
   }
   return NULL;
}

int main(int argc, char *argv[]){
   int port = atoi(argv[1]);

   fd = socket(AF_INET, SOCK_STREAM, 0);

   signal(SIGINT, handler);

   struct sockaddr_in server_addr;
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(port);
   server_addr.sin_addr.s_addr = INADDR_ANY;

   bind(fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
   
   char *ip = inet_ntoa(server_addr.sin_addr);
   printf("Server IP: %s\n", ip);

   struct sockaddr_in sin;
   socklen_t len = sizeof(sin);
   getsockname(fd, (struct sockaddr *)&sin, &len);
   printf("Port number %d\n", ntohs(sin.sin_port));

   listen(fd, MAX_CLIENTS);

   pthread_create(&alive_thread, NULL, ping, &fd);

   pthread_create(&thread, NULL, accept_clients, &fd);

   pthread_join(thread, NULL);

   pthread_join(alive_thread, NULL);

   return 0;
 }