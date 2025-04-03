#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    int n = strtol(argv[1], NULL, 10);
    for(int i = 0; i < n; i++){
        int p = fork();
        if (p == 0){
            printf("PPID: %d PID: %d\n", getppid(), getpid());
            return 0;
        }
    }

    while((wait(NULL)) > 0); 
    printf("%d\n", n);
    return 0;
}