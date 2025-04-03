#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFOR_SIZE 1024

void reverse(char word[]){
    int len = strlen(word);
    if(word[len - 1] == '\n') len--;
    for(int i = 0; i < len/2; i++){
        char tmp = word[i];
        word[i] = word[len - i - 1];
        word[len - i - 1] = tmp;
    }

}

int main(){
    char folder1[BUFFOR_SIZE/2];
    char folder2[BUFFOR_SIZE/2];

    printf("Podaj nazwe folderu wejściowego: ");
    scanf("%s", folder1);
    printf("Podaj nazwe folderu wyjściowego: ");
    scanf("%s", folder2);

    DIR* d_i = opendir(folder1);
    DIR* d_o = opendir(folder2);
    if (!d_o) mkdir(folder2, 0777);

    struct dirent* dir;
    int len;

    while((dir = readdir(d_i)) != NULL){
        char* name = dir->d_name;
        len = strlen(name);

        if(len >= 4){
            name += len - 4;
            if(strcmp(name, ".txt") == 0){
                char f_name[BUFFOR_SIZE];
                sprintf(f_name, "%s/%s", folder1, dir->d_name);
                FILE *f = fopen(f_name, "r");
                if(f == NULL){
                    printf("Nie udało się otworzyć pliku %s\n", f_name);
                    continue;
                }

                char f_name_o[BUFFOR_SIZE];
                sprintf(f_name_o, "%s/%s", folder2, dir->d_name);
                FILE *f_o = fopen(f_name_o, "w");
                if(f_o == NULL){
                    printf("Nie udało się otworzyć pliku %s\n", f_name_o);
                    continue;
                }
                
                char buff[BUFFOR_SIZE];
                while(fgets(buff, BUFFOR_SIZE, f) != NULL){
                    reverse(buff);
                    fprintf(f_o, "%s", buff);
                }
                fclose(f);
                fclose(f_o);
            }
        }
    }

    closedir(d_i);
    closedir(d_o);
    return 0;
}