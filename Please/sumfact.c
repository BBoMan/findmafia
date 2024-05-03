#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include<unistd.h>
#include<pthread.h>

void *threadFact(void *N) { //factorial function
    int num = atoi(N);
    int factorial = 1;  //variable of factorial
    for(int i=1; i<=num; i++) { //factorial of 1~N
        factorial *= i;
    }
    printf("sumfact : factorial = %d\n", factorial); // print factorial result

    return NULL;
}

void *threadSum(void *N) {  //sumation function
    int num = atoi(N);
    int sum = 0;            //variable of sumation
    for(int i=1; i<=num; i++) { //sumation of 1~N
        sum += i;
    }
    printf("sumfact : sum = %d\n", sum); //print sumation result
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tid[atoi(argv[1])];   //array size is parameter num

    pthread_create(&tid[0], NULL, threadFact, (void *)argv[1]); //first thread call factorial function
    pthread_join(tid[0], NULL); //sumfact process can't finish until this thread finish

    for(int i=1; i<atoi(argv[1]); i++) {
        pthread_create(&tid[i], NULL, threadSum, (void *)argv[1]); //extra thread call sumation function
    }

    for(int i=1; i<atoi(argv[1]); i++) { //until all thread finish, sumfact process can't finish
        pthread_join(tid[i], NULL);
    }
    return 0;
}