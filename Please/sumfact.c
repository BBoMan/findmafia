#include<stdio.h>
#include <sys/types.h>
#include<unistd.h>
#include<pthread.h>

void *threadFact(void *N) {
    int num = atoi(N);
    int factorial = 1;
    for(int i=1; i<=num; i++) {
        factorial *= i;
    }
    printf("sumfact : factorial = %d\n", factorial);
}

void *threadSum(void *N) {
    int num = atoi(N);
    int sum = 0;
    for(int i=1; i<=num; i++) {
        sum += i;
    }
    printf("sumfact : sum = %d\n", sum);
}

int main(int argc, char *argv[]) {
    pthread_t tid[atoi(argv[1])];

    pthread_create(&tid[0], NULL, threadFact, (void *)argv[1]);

    for(int i=1; i<atoi(argv[1]); i++) {
        pthread_create(&tid[i], NULL, threadSum, (void *)argv[1]);
    }

    for(int i=0l i<atoi(argv[1]); i++) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}