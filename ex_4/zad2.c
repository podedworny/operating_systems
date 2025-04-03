#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int global=0;

int main(int argc, char* argv[]){
    printf("%s\n", argv[0]);
    int local = 0;
    int child_pid = fork();
    if (child_pid == 0){
        printf("child process\n");
        local++;
        global++;
        printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("child's local = %d, child's global = %d\n", local, global);
        return execl("/bin/ls", "ls", "-l", argv[1], NULL);       
    } else {
        int p;
        wait(&p);
        int child_status = WEXITSTATUS(p);

        printf("parent process\n");
        printf("parent pid = %d, child pid = %d\n", getpid(), child_pid);
        printf("Child exit code: %d\n", child_status);
        printf("Parent's local = %d, parent's global = %d\n", local, global);
        return child_status;
    }
}