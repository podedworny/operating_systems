#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>
#include <stddef.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <wait.h>

int loop = 1;

void sig_handler() {
    loop = 0;
}

char* generate_random() {
    char *random = malloc(11);
    for (int i = 0; i < 10; i++) {
        random[i] = 'A' + rand() % 26;
    }
    random[10] = '\0';
    return random;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);

    int no_of_users = atoi(argv[1]);

    sem_t *sem = sem_open("waiting", O_RDWR);
    sem_t *search_sem = sem_open("search", O_RDWR | O_CREAT, 0666, 1);

    int no_of_printers;
    sem_getvalue(sem, &no_of_printers);

    int shm_fd = shm_open("/shared_memory", O_RDWR, 0666);
    char (*shm_ptr)[11] = mmap(NULL, no_of_printers * 11, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);


    for (int i = 0; i < no_of_users; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            srand(time(NULL)+i);

            while (loop){
                sem_wait(sem);

                sem_wait(search_sem);
                int j = 0;

                while (j < no_of_printers && shm_ptr[j][0] != '\0') j++;

                char* random = generate_random();
                printf("User %d: %s\n", i, random);
                strcpy(shm_ptr[j], random);
                fflush(stdout);
                sem_post(search_sem);


                sleep(rand() % 3 + 15);
            }
        exit(0);
        }
    }
    while(wait(NULL) > 0) {}
    
    munmap(shm_ptr, no_of_printers * 11);
    sem_close(sem);


    return 0;
}