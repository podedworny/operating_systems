#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int counter = 0;


void handler(int sig, siginfo_t *si, void *ucontext){
    kill(si->si_pid, SIGUSR1);
    switch (si->si_value.sival_int){
        case 1:
        for(int i = 1; i <= 100; i++)
            printf("%d\n", i);
        break;

        case 2:
        printf("%d\n", counter);
        break;

        case 3:
        exit(0);
    }
    counter++;
}

int main(){
    printf("PID: %d\n", getpid());
    struct sigaction act = {0};

    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &act, NULL);
    while(1);   
}