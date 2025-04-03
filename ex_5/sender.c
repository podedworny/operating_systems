#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler(){
    printf("Signal received\n");
}

int main(int argc, char *argv[]) {
    signal(SIGUSR1, handler);

    union sigval value;
    value.sival_int = atoi(argv[2]);
    sigqueue(atoi(argv[1]), SIGUSR1, value);



    sigset_t mask;
    sigfillset(&mask);

    sigdelset(&mask, SIGUSR1);
    sigdelset(&mask, SIGINT);
    sigsuspend(&mask);

    return 0;
}