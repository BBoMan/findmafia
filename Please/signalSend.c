#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>

#define MAX_PROCESS 201 // 생성되는 프로세서의 개수가 200개 이기 때문

// "linuxtown_class" 실행 파일에 의해 생성된 모든 프로세스의 PID를 얻는 함수
int getPIDs(pid_t *pids) {
    DIR *dir;
    struct dirent *entry;
    int cnt = 0;

    // /proc 디렉토리 열기
    dir = opendir("/proc");
    if (dir == NULL) {
        perror("Error opening /proc directory");
        return -1;
    }

    // /proc 디렉토리 내의 각 디렉토리에 대해 처리
    while ((entry = readdir(dir)) != NULL) {
        // 현재 entry가 diretory인지 확인, /proc directory 내의 각 프로세스의 정보가 directory 형태로 존재하기 때문
        if (entry->d_type == DT_DIR) {
            // 디렉토리 이름이 숫자인 경우 (프로세스의 PID)
            char *endptr;
            // PID값이 문자열로 나오기 때문에 10진수 정수로 변환해줌
            // &endptr은 오류 검출을 위해 있음
            long pid = strtol(entry->d_name, &endptr, 10);

            // PID값에 오류가 없으면
            if (*endptr == '\0') {
                // PID에 해당하는 프로세스의 cmdline 파일 열기
                char filename[256];
                FILE *fp;
                // filename에 프로세스의 명령 라인 인수를 넣어줌
                snprintf(filename, sizeof(filename), "/proc/%ld/cmdline", pid);
                fp = fopen(filename, "r");
                if (fp != NULL) {
                    char cmdline[256];
                    // cmdline 파일에서 프로세스의 실행 파일 읽기
                    if (fgets(cmdline, sizeof(cmdline), fp) != NULL) {
                        // PID에 해당하는 프로세스의 명령 라인에 linuxtown_class라는 문자열이 있는지 검색
                        if (strstr(cmdline, "linuxtown_class") != NULL) {
                            //pid_t 배열에 pid값 넣기 / sendSIGUSR1() 함수를 이용하기 위함
                            pids[cnt++] = (pid_t)pid;
                        }
                    }
                    fclose(fp);
                }
            }
        }
    }

    closedir(dir);
    //이후 반복문을 몇번 반복할지 정하기 위함
    return cnt;
}

//결과 값으로 찾아낸 pid값을 return
void sendSIGUSR1(pid_t pid) {
    int pidNum;
    long result;
    //PID가 pid인 프로세스에 kill -10 (SIGUSR1 시그널을 보냄)
    if (kill(pid, 10) == 0) {

    } else {
        perror("Error sending SIGUSR1 signal");
        exit(-1);
    }
}

int findMafia(int *mafia) {
    //confession.txt 파일의 결과는 !!! 혹은 ... 인 것으로 확인했기 때문
    char evi[4];
    int cnt=0;

    //confession.txt 파일 열기
    FILE *eviFp = fopen("confession.txt", "r");
    if (eviFp == NULL) {
        //오류 있으면 -1 return
        perror("file open error");
        return -1;
    }
    
    for(int i=0; fgets(evi, sizeof(evi), eviFp) != NULL; i++) {
        if (strstr(evi, "!") != NULL) {
            mafia[cnt++] = i;
        }
    }
    return cnt;
}

int main() {
    pid_t pids[MAX_PROCESS];    //linuxtown의 모든 사람들의 pid를 저장
    pid_t pid;
    int numPeople;
    int mafia[MAX_PROCESS]; //mafia의 pid를 저장
    int numMafia;

    // linuxtown_class 실행파일에 의해서 생성된 프로세스의 PID를 pids 배열에 넣고
    // 그 개수를 numPeople에 return
    numPeople = getPIDs(pids);
    if (numPeople == -1) {
        return -1;
    }

    // linuxtown_class 실행파일에 의해서 생성된 프로세스의 PID에 해당하는 프로세스에 SIGUSR1 signal 보내기
    // confession.txt 파일 완성하기
    for (int i = 0; i < numPeople; i++) {
        pid=pids[i];
        sendSIGUSR1(pid);
    }

    numMafia = findMafia(mafia);
    if (numMafia == -1) {
        return -1;
    }

    printf("mafia = %d\n", numMafia);
    printf("citizen = %d\n", numPeople-numMafia);
    printf("=== mafia list ==\n\n");

    for(int i=0; i<numMafia; i++) {
        //테스트를 통해, confession.txt 파일에 !!! or ... 이 추가되는 방식이
        //SIGUSR1을 받으면 마지막 부분부터 채워지는 것을 확인.
        printf("%d\n", pids[mafia[i]]);
    }

    return 0;
}