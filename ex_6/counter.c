#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>

int main(){
    mkfifo("pipe", 0666);
    int fdo = open("pipe", O_RDONLY);
    int x, y, n;
    read(fdo, &x, sizeof(int));
    read(fdo, &y, sizeof(int));
    read(fdo, &n, sizeof(int));
    close(fdo);
    float h = (float)(y - x) / n;
    float start = x + h / 2;
    int fd[2];
    pipe(fd);
    while (start < y){
        pid_t pid = fork();
        if (pid == 0){
            float j = h * 4 /(start * start + 1);
            close(fd[0]);
            write(fd[1], &j, sizeof(float));
            exit(0);
        }
        start += h;
    }
    while(wait(NULL) > 0);
    close(fd[1]);

    float numbers[n];
    read(fd[0], numbers, sizeof(float) * n);

    float result = 0.0;
    for (int i=0; i<n; i++){
        result += numbers[i];
    }

    fdo = open("pipe", O_WRONLY);
    write(fdo, &result, sizeof(float));
    close(fdo);
    unlink("pipe");
    return 0;
    
}