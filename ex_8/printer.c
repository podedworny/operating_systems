#define _XOPEN_SOURCE 700
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
#include <sys/types.h>
#include <wait.h>
#include <string.h>

int loop = 1;

void sig_handler() {
    loop = 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, sig_handler);

    int no_of_printers = atoi(argv[1]);

    sem_t *sem = sem_open("/waiting", O_RDWR | O_CREAT, 0666, no_of_printers);

    int shm_fd = shm_open("/shared_memory", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, no_of_printers * 11);
    char (*shm_ptr)[11] = mmap(NULL, no_of_printers * 11, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    memset(shm_ptr, 0, no_of_printers * 11);

    for (int i = 0; i < no_of_printers; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            while (loop){
                if (shm_ptr[i][0] != 0) {
                    for (int j = 0; j < 10; j++){
                        if (!loop) break;
                        
                        printf("Printer %d: %c\n", i, shm_ptr[i][j]);
                        sleep(1);
                    }
                    sem_post(sem);
                    memset(shm_ptr[i], 0, 11);
                }
            }
        }
    }
    while(wait(NULL) > 0) {}

    munmap(shm_ptr, no_of_printers * 11);
    sem_close(sem);
    shm_unlink("/shared_memory");
    sem_unlink("/waiting");
    return 0;
}