#include <stdio.h>
#include <signal.h>
#include <string.h>

void handler() {
    printf("Otrzymano sygnał\n");
    return;
}

int main(int argc, char *argv[]) {
    if (strcmp(argv[1], "none") == 0){
        signal(SIGUSR1, SIG_DFL);
    } else if (strcmp(argv[1], "ignore") == 0) {
        signal(SIGUSR1, SIG_IGN);
    } else if(strcmp(argv[1], "handler") == 0) {
        signal(SIGUSR1, handler);
    } else if(strcmp(argv[1], "mask") == 0) {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &mask, NULL);

    } 

    raise(SIGUSR1);
    if (strcmp(argv[1], "mask") == 0) { 
       sigset_t pending;
        sigpending(&pending);
        if (sigismember(&pending, SIGUSR1)) {
            printf("Sygnał SIGUSR1 jest oczekujący\n");
        } else {
            printf("Sygnał SIGUSR1 nie jest oczekujący\n");
        }
    }

    return 0;
}