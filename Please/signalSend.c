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
    int count = 0;

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
                            pids[count++] = (pid_t)pid;
                        }
                    }
                    fclose(fp);
                }
            }
        }
    }

    closedir(dir);
    //이후 반복문을 몇번 반복할지 정하기 위함
    return count;
}

void sendSIGUSR1(pid_t pid) {
    //PID가 pid인 프로세스에 kill -10 (SIGUSR1 시그널을 보냄)
    if (kill(pid, 10) == 0) {
        
    } else {
        perror("Error sending SIGUSR1 signal");
        exit(EXIT_FAILURE);
    }
}

int main() {
    pid_t pids[MAX_PROCESS];
    pid_t pid;
    int numProcess;

    // linuxtown_class 실행파일에 의해서 생성된 프로세스의 PID를 pids 배열에 넣고
    // 그 개수를 numProcess에 return
    numProcess = getPIDs(pids);
    if (numProcess == -1) {
        return EXIT_FAILURE;
    }

    // linuxtown_class 실행파일에 의해서 생성된 프로세스의 PID에 해당하는 프로세스에 SIGUSR1 signal 보내기
    for (int i = 0; i < numProcess; i++) {
        pid=pids[i];
        sendSIGUSR1(pid);
    }

    return EXIT_SUCCESS;
}