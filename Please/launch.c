#include<stdio.h>
#include<stdlib.h>
#include <sys/types.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char *argv[]) {
    int pid;
    if((pid = fork()) == 0) {   //child part
        printf("launch : start sumfact %s\n", argv[1]); //sumfact print
        execlp("./sumfact", "sumfact", argv[1], NULL); //start sumfact with parameter 
        printf("execlp failed!\n");    //if execlp() failed, print the message
        return -1;
    }
    else if(pid > 0) {  //parent part
        int sum = 0;    //variable of sumation
        for(int i=0; i < atoi(argv[1]); i++) { //sumation of 1~N
            sum += (i+1);
        }
        printf("laundch : sum = %d\n", sum);    //print the total sum
        waitpid(pid, NULL, 0);                  //wait until child finish
        return 0;
    }
    else {
        printf("fork() is failed\n");           //fork failed
        return -1;
    }
}