#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);
    int n = atoi(argv[3]);

    int fdo = open("pipe", O_WRONLY);
    write(fdo, &x, sizeof(int));
    write(fdo, &y, sizeof(int));
    write(fdo, &n, sizeof(int));
    close(fdo);

    float result;
    fdo = open("pipe", O_RDONLY);
    read(fdo, &result, sizeof(float));
    close(fdo);
    printf("wynik: %f\n", result);
    return 0;
}