#include<stdio.h>
#include <sys/types.h>
#include<unistd.h>

int main(int argc, char *argv[]) {
    int pid;
    if((pid = fork()) == 0) {   //child part
        printf("launch : start sumfact %s\n", argv[1]);
        execlp("sumfact", argv[1], NULL); //start sumfact, 
        panic("execlp failed!");    //if execlp() failed, print the message
        return -1;
    }
    else if(pid > 0) {  //parent part
        int sum = 0;    //total sum of 1~N
        for(int i=0; i < atoi(argv[1]); i++) {
            sum += (i+1);
        }
        printf("laundch : sum = %d\n", sum);    //print the sum
        return 0;
    }
    else {
        printf("fork() is failed\n");
        return -1;
    }
}