#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//fifo
typedef struct {
    int *items;                                         // 동적으로 할당된 배열
    int front;                                          // 첫 번째 요소의 인덱스
    int rear;                                           // 마지막 요소의 인덱스
    int size;                                           // 현재 큐의 크기
    int max;                                            // 큐의 최대 크기
} Queue;

void initQueue(Queue *q, int frameSize) {               // 큐 초기화 함수
    q->items = (int *)malloc(sizeof(int) * frameSize);  // 큐의 크기에 맞게 동적 할당
    q->front = -1;                                      // front 초기화
    q->rear = -1;                                       // rear 초기화
    q->size = 0;                                        // size 초기화
    q->max = frameSize;                                 // 최대 크기 설정
}

void addQueue(Queue *q, int value) {                    // 큐에 요소 추가 함수
    if (q->size == q->max) {                            // 큐가 가득 차면
        q->front = (q->front + 1) % q->max;             // 가장 오래된 요소를 제거
    } else {
        q->size++;                                      // 큐 크기 증가
    }
    q->rear = (q->rear + 1) % q->max;                   // rear 갱신
    q->items[q->rear] = value;                          // 새로운 요소 추가
}

bool existQueue(Queue *q, int value) {                  // 큐에 요소가 존재하는지 확인하는 함수
    int i = q->front;                                   // front부터 시작
    for (int count = 0; count < q->size; count++) {     // 큐의 크기만큼 반복
        if (q->items[i] == value) {                     // 값이 존재하면
        return true;                                    // true 반환
        }
        i = (i + 1) % q->max;                           // 다음 인덱스로 이동
    }
    return false;                                       // 값이 없으면 false 반환
}

void addFifo(Queue *q, int value, int *hit, int *fault) {// FIFO 알고리즘을 이용하여 페이지를 추가하는 함수
    if (existQueue(q, value)) {                         // 값이 존재하면
        (*hit)++;                                       // 히트 증가
    } else {
        addQueue(q, value);                             // 큐에 값 추가
        (*fault)++;                                     // 페이지 폴트 증가
    }
}

//NRU
// 노드 구조체 정의
struct Node {
    int pageNum;
    int R;
    int M;
    int class;
    struct Node* next;
};

// 새로운 노드를 생성하는 함수, read, write 결정되어서 들어옴
struct Node* createNode(int pageNum, char* operator) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));//newNode에 메모리 할당
    if (strcmp(operator, "read") == 0) {      //operator가 read면
        newNode->pageNum = pageNum;             //pageNumber 등록하고
        newNode->R = 0;                         //R bit = 0
        newNode->M = 0;                         //M bit = 0
        newNode->class = 0;                     //따라서 class = 0
        newNode->next = NULL;                   //새로운 노드이기 때문에 next = NULL
        return newNode;                         //새로운 노드 리턴
    }
    else if (strcmp(operator, "write") == 0) {//operator가 write면
        newNode->pageNum = pageNum;             //pageNumber 등록하고
        newNode->R = 0;                         //R bit = 0
        newNode->M = 1;                         //M bit = 1
        newNode->class = 1;                     //따라서 class = 1
        newNode->next = NULL;                   //새로운 노드이기 때문에 next = NULL
        return newNode;                         //새로운 노드 리턴
    }
    else {                                      //operator가 read, write가 아니면 오류
        printf("operator error\n");
        free(newNode);                          //메모리 할당 해제
        return NULL;                            //NULL 반환
    }
}

// 연결 리스트에 노드 추가 함수
void addNode(struct Node** head, int pageNum, char* operator) {
    struct Node* newNode = createNode(pageNum, operator);//노드를 추가할 상황에만 부름, newNode에 메모리 할당
    struct Node* last = *head;                           //연결 리스트의 마지막까지 이동하기 위해 head값을 복사
    if (last == NULL) {                                  //연결리스트가 비었으면
        *head = newNode;                                 //헤드에 새노드 추가
        return;                                          //addNode() 함수 종료
    }
    while (last->next != NULL) {                         //연결 리스트의 마지막 까지 이동
        last = last->next;                               //last를 한칸씩 이동
    }
    last->next = newNode;                                //새로운 노드를 연결 리스트의 마지막에 추가
}

// 노드를 삭제하는 함수
void delNode(struct Node** head, int minClass) {
    struct Node* temp = *head;                      //class가 minClass인 노드를 삭제하기 위해 head값을 복사
    struct Node* prev = NULL;                       //삭제할 node의 이전 노드를 알기 위한 노드

    if (temp != NULL && temp->class == minClass) {  //헤드 노드 자체가 삭제할 노드인 경우
        *head = temp->next;                         //헤드를 다음 노드로 변경
        temp->next = NULL;                          //삭제 노드와 연결된 노드를 지우고
        free(temp);                                 //메모리 할당을 해제
        return;                                     //delNode() 함수를 종료
    }

    while (temp != NULL && temp->class != minClass) {// 삭제할 노드를 찾을 때까지 리스트를 순회
        prev = temp;                                //이전 노드에 현재 노드 값 복사
        temp = temp->next;                          //현재 노드에 다음 노드 값 복사
    }
    prev->next = temp->next;                        //이전 노드의 next를 현재의 next로 설정
    temp->next = NULL;                              //삭제 노드와 연결된 노드를 지우고
    free(temp);                                     //메모리 할당을 해제
    return;                                         //delNode() 함수를 종료
}
// 상태 bit에 따라 class를 리턴
int checkClass(int R, int M) {                      //R bit와 M bit에 따라 class를 결정
    if(R == 0 && M == 0)                            //00 이면 class = 0
        return 0;
    else if(R == 0 && M == 1)                       //01 이면 class = 1
        return 1;
    else if(R == 1 && M == 0)                       //10 이면 class = 2
        return 2;
    else if(R == 1 && M == 1)                       //11 이면 class = 3
        return 3;
}

// 새로 들어온 pageNum에 따라 page를 replace한다.
void addNru(struct Node** head, int pageNum, char* operator, int *cntNode, int frameSize, int *hit, int *fault) {
    struct Node* temp = *head;                         //새로 들어온 pageNum을 가진 노드가 있는지 확인하기 위해 head값을 복사
    struct Node* prev = NULL;                          //삭제할 node의 이전 노드를 알기 위한 노드
    struct Node* last = *head;                         //연결 리스트의 마지막까지 이동하기 위해 head값을 복사
    int minClass = 3;                                  //minClass 값을 3으로 초기화

    if (temp != NULL && temp->next != NULL) {
        while (last->next != NULL) {                    //연결 리스트의 마지막 까지 이동
            if (last->class < minClass)                 //노드의 class가 minClass보다 작을 경우
                minClass = last->class;                 //minClass 값을 최신화
            last = last->next;   
        }
         if (last->class < minClass)                    //마지막 last의 class는 minClass에 반영 안됨
            minClass = last->class;                     //마지막 last의 class도 minClass에 반영

        struct Node* fLast = last;                         // 초기 last를 기억

        while (temp != NULL && temp->pageNum != pageNum) {  //조건에 맞는 것 있는지 검색
            prev = temp;
            temp = temp->next;
        }        

        if (temp == NULL) {                                //조건에 맞는 노드가 없을 때
            if (*cntNode < frameSize) {                    //Node수가 frameSize보다 적은 경우
                addNode(head, pageNum, operator);          //연결 리스트에 노드 추가
                (*fault)++;                                //fault를 1 추가
                (*cntNode)++;                              //Node 수를 1 추가
                return;                                    //addNru()함수를 종료
            }
            else {                                         //Node수가 frameSize보다 큰 경우
                delNode(head, minClass);                   //연결 리스트의 노드중 class = minClass에 해당하는 첫번째 노드를 삭제
                addNode(head, pageNum, operator);          //연결 리스트에 노드 추가
                (*fault)++;                                //fault를 1 추가
                return;                                    //addNru()함수를 종료
            }
        }

        if (temp == fLast) {                           //조건에 맞는 노드가 마지막 노드인 경우
            if (strcmp(operator, "write") == 0) {    //operator가 write 였다면
                temp->R = 1;                           //R = 1
                temp->M = 1;                           //M = 1
                temp->class = 3;                       //class = 3
            }
            else if (strcmp(operator, "read") == 0) {//operator가 read 였다면
                temp->R = 1;                           //R = 1
                temp->class = checkClass(temp->R, temp->M);//class값을 확인함              
            }
            (*hit)++;                                   //hit를 1 추가
            return;
        }

        if (prev != NULL) {
            prev->next = temp->next;                 //temp 연결 끊고
        } else {
            *head = temp->next;                      //head를 바꿈
        }

        temp->next = NULL;                           //next에 NULL
        last->next = temp;                           //끝으로 이동
        if (strcmp(operator, "write") == 0) {    //operator가 write 였다면
            temp->R = 1;                           //R = 1
            temp->M = 1;                           //M = 1
            temp->class = 3;                       //class = 3
        }
        else if (strcmp(operator, "read") == 0) {//operator가 read 였다면
            temp->R = 1;                           //R = 1
            temp->class = checkClass(temp->R, temp->M);//class값을 확인함              
        }
        (*hit)++;                                   //hit를 1 추가
        return;
    }
    else if (temp == NULL) {                       //연결리스트가 비어 있을 때
        addNode(head, pageNum, operator);          //연결 리스트에 노드 추가
        (*fault)++;                                //fault를 1 추가
        (*cntNode)++;                              //Node 수를 1 추가
        return;
    }
    else if (temp->next == NULL) {                 //연결리스트에 하나의 노드만 있을 때
        if (temp->pageNum == pageNum) {
            if (strcmp(operator, "write") == 0) {    //operator가 write 였다면
                temp->R = 1;                           //R = 1
                temp->M = 1;                           //M = 1
                temp->class = 3;                       //class = 3
            }
            else if (strcmp(operator, "read") == 0) {//operator가 read 였다면
                temp->R = 1;                           //R = 1
                temp->class = checkClass(temp->R, temp->M);//class값을 확인함              
            }
            (*hit)++;                                   //hit를 1 추가
            return;  
        }
        else {
            addNode(head, pageNum, operator);          //연결 리스트에 노드 추가
            (*fault)++;                                //fault를 1 추가
            (*cntNode)++;                              //Node 수를 1 추가
            return;
        }
    }
}

int main(int argc, char *argv[]) {  //argv[1] = fifo or nru / argv[2] = number of frame
    char *operator;                 // 연산자 변수
    int pageNum = 0;                // 페이지 번호 변수
    int frameSize = atoi(argv[2]);  // 프레임 크기 설정
    int access = 0;                 // 접근 횟수
    int read = 0;                   // 읽기 횟수
    int write = 0;                  // 쓰기 횟수
    int hit = 0;                    // 히트 수
    int fault = 0;                  // 페이지 폴트 수
    char line[12];                  // 입력 라인 저장 변수

    struct Node* head = NULL;       // NRU 알고리즘을 위한 노드 리스트 헤드
    int minClass = 3;               // 최소 클래스 값
    int cntNode = 0;                // 노드 수

    FILE *file = fopen("access.list", "r");// 파일 열기
    if (strcmp(argv[1], "fifo") == 0 ) {   // FIFO 알고리즘 선택 시
        Queue q;                           // 큐 생성
        initQueue(&q, frameSize);          // 큐 초기화

        while (fgets(line, sizeof(line), file)) { // 파일에서 한 줄씩 읽기
        access++;                                 // 접근 횟수 증가
        pageNum = atoi(strtok(line, " "));        // 페이지 번호 파싱
        operator = strtok(NULL, " ");             // 연산자 파싱

        operator[strcspn(operator, "\r\n")] = 0;

        if (strcmp(operator, "read") == 0)      // read 연산자 처리
            read++;                               // 읽기 횟수 증가
        else if (strcmp(operator, "write") == 0)// write 연산자 처리
            write++;                              // 쓰기 횟수 증가
        addFifo(&q, pageNum, &hit, &fault);       // FIFO 페이지 추가
        }
        free(q.items);                            // 큐 메모리 해제
    }

    else if (strcmp(argv[1], "nru") == 0 ) {      // NRU 알고리즘 선택 시
        while (fgets(line, sizeof(line), file)) { // 파일에서 한 줄씩 읽기
        access++;                                 // 접근 횟수 증가d
        pageNum = atoi(strtok(line, " "));        // 페이지 번호 파싱
        operator = strtok(NULL, " ");             // 연산자 파싱

        operator[strcspn(operator, "\r\n")] = 0;

        addNru(&head, pageNum, operator, &cntNode, frameSize, &hit, &fault);  // NRU 페이지 추가
        if (strcmp(operator, "read") == 0)      // read 연산자 처리
            read++;                               // 읽기 횟수 증가
        else if (strcmp(operator, "write") == 0)// write 연산자 처리
            write++;                              // 쓰기 횟수 증가
        }
    }
    fclose(file);

    printf("Total number of access: %d\n", access);
    printf("Total number of read: %d\n", read);
    printf("Total number of write: %d\n", write);
    printf("Number of page hits: %d\n", hit);
    printf("Number of page faults: %d\n", fault);
    printf("Page fault rate: %d/%d = %.2f%%\n",fault, access, ((float)fault / (float)access)*100);
    printf("frame size : %d\n", cntNode);

    return 0;
}