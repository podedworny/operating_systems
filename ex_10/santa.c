#include <pthread.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/types.h>

#define REINDEERS_COUNT 9
#define ITERATIONS 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

int N = 0;


void* santa_sleep(void* arg){
    pthread_t* reindeers = (pthread_t*)arg;

    for (int i=0; i<ITERATIONS; i++){        
        pthread_mutex_lock(&mutex);

        while (N != REINDEERS_COUNT){
            pthread_cond_wait(&cond, &mutex);
        }
        
        pthread_mutex_unlock(&mutex);

        printf("Mikołaj: budzę się\n");
        printf("Mikołaj: dostarczam zabawki\n");
        sleep(rand() % 3 + 2);
        printf("Mikołaj: zasypiam\n");

        pthread_mutex_lock(&mutex);
        N = 0;
        pthread_mutex_unlock(&mutex);

        pthread_cond_broadcast(&cond2);
    }

    for (int i=0; i<REINDEERS_COUNT; i++){
        pthread_cancel(reindeers[i]);
    }

    return NULL;
}

void* holidays(void* arg){
    int id = *((int*)arg);

    while (1) {
        sleep(rand() % 6 + 5);

        pthread_mutex_lock(&mutex);
        N++;

        printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", N, id);

        if (N == REINDEERS_COUNT) {
            printf("Renifer: wybudzam Mikołaja, %d\n", id);
            pthread_cond_signal(&cond);
        }

        while (N > 0) {
            pthread_cond_wait(&cond2, &mutex);
        }
        
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}

int main(){
    pthread_t reindeers[REINDEERS_COUNT];
    int ids[REINDEERS_COUNT];
    pthread_t santa;

    for (int i=0;i<REINDEERS_COUNT; i++){
        ids[i] = i;
        pthread_create(&reindeers[i], NULL, holidays, &ids[i]);
    }
    
    pthread_create(&santa, NULL, santa_sleep, reindeers);
    pthread_join(santa, NULL);

    for (int i=0; i < REINDEERS_COUNT; i++){
        pthread_join(reindeers[i], NULL);
    }

    return 0;
}