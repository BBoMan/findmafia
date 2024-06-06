#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 

#define MAX_PAGES 1000 // 최대 페이지 수
#define MAX_ACCESSES 50000 // access의 총 개수가 50000개(read + write)

// 페이지 요청 구조체
typedef struct{
    int page_id; // 페이지 번호
    char action[6]; // 작업 종류 (read/write)
} PageRequest;

// 프레임 구조체 
typedef struct{
    int page_id; // 페이지 번호
    int ref_bit; // 참조 비트
    int mod_bit; // 수정 비트
    int last_access; // 마지막 접근 시간
} Frame;

// 큐 노드 구조체 
typedef struct QueueNode{
    int page_id; // 페이지 번호
    struct QueueNode* next; // 다음 노드 포인터
} QueueNode;

// 큐 구조체 
typedef struct{
    QueueNode* head; // 큐의 헤드 노드 포인터
    QueueNode* tail; // 큐의 테일 노드 포인터
    int current_size; // 현재 큐의 크기
    int max_capacity; // 큐의 최대 크기
} Queue;

// 큐 생성 함수
Queue* createQueue(int capacity){
    Queue* q = (Queue*)malloc(sizeof(Queue)); // 큐를 위한 메모리 할당
    q->max_capacity = capacity; // 최대 크기 설정
    q->head = q->tail = NULL; // 헤드와 테일 null로 초기화
    q->current_size = 0; // 현재 크기 초기화
    return q; // 생성된 큐 반환
}

// 큐가 가득 찼는지 확인하는 함수
int isQueueFull(Queue* q)
{
    return q->current_size == q->max_capacity; // 현재 크기와 최대 크기 비교
}

// 큐가 비어있는지 확인하는 함수
int isQueueEmpty(Queue* q)
{
    return q->current_size == 0; // 현재 크기가 0인지 확인
}

// 큐에 요소를 추가하는 함수
void enqueue(Queue* q, int page_id)
{
    if (isQueueFull(q)) // 큐가 가득 찼다면
    {
        return; // 함수 종료
    }
    QueueNode* new_node = (QueueNode*)malloc(sizeof(QueueNode)); // 새로운 노드 생성
    new_node->page_id = page_id; // 노드에 페이지 번호 설정
    new_node->next = NULL; // 다음 노드를 NULL로 설정
    if (q->tail == NULL) // 큐가 비어있다면
    {
        q->head = q->tail = new_node; // 헤드와 테일을 새로운 노드로 설정
    }
    else
    {
        q->tail->next = new_node; // 테일의 다음 노드를 새로운 노드로 설정
        q->tail = new_node; // 테일을 새로운 노드로 업데이트
    }
    q->current_size++; // 현재 크기 증가
}

// 큐에서 요소를 제거하는 함수
int dequeue(Queue* q)
{
    if (isQueueEmpty(q)) // 큐가 비어있다면
    {
        return -1; // -1 반환
    }
    QueueNode* temp = q->head; // 헤드 노드를 임시 변수 temp에 저장
    int page_id = temp->page_id; // 헤드 노드의 페이지 번호 저장
    q->head = q->head->next; // 헤드를 다음 노드로 설정
    if (q->head == NULL) // 헤드가 NULL이라면
    {
        q->tail = NULL; // 테일도 NULL로 설정
    }
    free(temp); // 임시 노드 temp 메모리 해제
    q->current_size--; // 현재 크기 감소
    return page_id; // 제거한 페이지 번호 반환
}

// FIFO 페이지 교체 알고리즘 함수
int fifoReplacement(PageRequest* requests, int total_requests, int num_frames) //num_frames = 입력받은 physical frame 수
{
    Queue* q = createQueue(num_frames); // 큐 생성
    int* frames = (int*)malloc(num_frames * sizeof(int)); // 프레임 배열 메모리 할당 및 초기화
    int page_faults = 0; // 페이지 폴트 수 초기화
    int page_hits = 0; // 페이지 히트 수 초기화
    memset(frames, -1, num_frames * sizeof(int)); // 프레임 배열 전부 -1로 초기화

    for (int i = 0; i < total_requests; i++) // 모든 요청(read + write)에 대해 반복
    {
        int page_id = requests[i].page_id; // 현재 요청의 페이지 번호 가져오기
        int hit = 0; // 히트 여부 0으로 초기화
        for (int j = 0; j < num_frames; j++) // 모든 프레임에 대해 반복
        {
            if (frames[j] == page_id) // 프레임에 페이지가 있다면
            {
                page_hits++; // 히트 수 증가
                hit = 1; // 히트 여부 1로 설정
                break; // 루프 종료
            }
        }

        if (!hit) // 페이지가 프레임에 없다면
        {
            page_faults++; // 페이지 폴트 수 증가
            if (isQueueFull(q)) // 큐가 가득 찼다면
            {
                int old_page = dequeue(q); // 큐에서 페이지 제거
                for (int j = 0; j < num_frames; j++) // 모든 프레임에 대해 반복
                {
                    if (frames[j] == old_page) // 제거한 페이지를 찾으면
                    {
                        frames[j] = -1; // 프레임을 비움
                        break; // 루프 종료
                    }
                }
            }
            enqueue(q, page_id); // 큐에 새로운 페이지 추가
            for (int j = 0; j < num_frames; j++) // 모든 프레임에 대해 반복
            {
                if (frames[j] == -1) // 빈 프레임을 찾으면
                {
                    frames[j] = page_id; // 페이지를 프레임에 넣음
                    break; // 루프 종료
                }
            }
        }
    }
    free(frames); // 프레임 배열 메모리 해제
    free(q); // 큐 메모리 해제
    return page_faults; // 페이지 폴트 수 반환
}

// 피해자 프레임 선택 함수 (NRU 알고리즘)
int selectVictim(Frame* frames, int num_frames)
{  /*클래스 0: 참조 비트가 0이고 수정 비트가 0인 프레임 => frame_class = 0
   클래스 1: 참조 비트가 0이고 수정 비트가 1인 프레임 => frame_class = 1
   클래스 2 : 참조 비트가 1이고 수정 비트가 0인 프레임 => frame_class = 2
   클래스 3 : 참조 비트가 1이고 수정 비트가 1인 프레임 => frame_class = 3 */

    int victim_index = -1; // 피해자 인덱스(교체되어야 할 페이지) -1로 초기화
    int min_class = 4; // 최소 클래스(=가장 우선순위가 높은 클래스) 4로 초기화
    for (int j = 0; j < num_frames; j++) // 모든 프레임에 대해 반복
    {
        int frame_class = frames[j].ref_bit * 2 + frames[j].mod_bit; // 프레임 클래스 계산
        if (frame_class < min_class || (frame_class == min_class && (victim_index == -1 ||
         frames[j].last_access < frames[victim_index].last_access))) // 최소 클래스 또는 동일한 클래스 중에서 가장 오래된 프레임이라면
        {
            victim_index = j; // 피해자 프레임(교체되어야 할 페이지) 인덱스 업데이트
            min_class = frame_class; // 최소 클래스 업데이트
        }
    }
    return victim_index; // 피해자 프레임(=교체되어야 할 페이지) 인덱스 반환
}

// NRU 페이지 교체 알고리즘 함수
int nruReplacement(PageRequest* requests, int total_requests, int num_frames)
{
    Frame* frames = (Frame*)malloc(num_frames * sizeof(Frame)); // 프레임 배열 메모리 할당
    int page_faults = 0; // 페이지 폴트 수 초기화
    int page_hits = 0; // 페이지 히트 수 초기화
    int time_stamp = 0; // 시간 스탬프 초기화

    for (int i = 0; i < num_frames; i++) // 모든 프레임에 대해 반복
    {
        frames[i].page_id = -1; // 페이지 번호 초기화
        frames[i].ref_bit = 0; // 참조 비트 초기화
        frames[i].mod_bit = 0; // 수정 비트 초기화
        frames[i].last_access = -1; // 마지막 접근 시간 초기화
    }

    for (int i = 0; i < total_requests; i++) // 모든 요청(read + write)에 대해 반복
    {
        int page_id = requests[i].page_id; // 현재 요청의 페이지 번호 가져오기
        char* action = requests[i].action; // 현재 요청의 작업 종류 가져오기
        int hit = 0; // 히트 여부 초기화

        for (int j = 0; j < num_frames; j++) // 모든 프레임에 대해 반복
        {
            if (frames[j].page_id == page_id) // 프레임에 페이지가 있다면
            {
                page_hits++; // 히트 수 증가
                frames[j].ref_bit = 1; // 참조 비트 설정
                if (strcmp(action, "write") == 0) // 작업이 쓰기라면
                {
                    frames[j].mod_bit = 1; // 수정 비트 1로 설정
                }
                frames[j].last_access = time_stamp++; // 마지막 접근 시간 업데이트
                hit = 1; // 히트 여부 설정
                break; // 루프 종료
            }
        }

        if (!hit) // 페이지가 프레임에 없다면
        {
            page_faults++; // 페이지 폴트 수 증가
            int victim_idx = selectVictim(frames, num_frames); // 피해자 프레임(=교체되어야 할 페이지) 선택

            frames[victim_idx].page_id = page_id; // 피해자 프레임(교체되어야 할 페이지)에 페이지 설정
            frames[victim_idx].ref_bit = 0; // 참조 비트 0으로 초기화
            frames[victim_idx].mod_bit = (strcmp(action, "write") == 0) ? 1 : 0; // write 연산이라면 수정 비트 1로 아니면 0으로 설정
            frames[victim_idx].last_access = time_stamp++; // 마지막 접근 시간 업데이트 
        }
    }

    free(frames); // 프레임 배열 메모리 해제
    return page_faults; // 페이지 폴트 수 반환
}

// 프로그램의 시작점
int main(int argc, char* argv[])
{
    if (argc != 3) // 인자가 3개가 아니면
    {
        printf("Usage: %s <algorithm> <frames>\n", argv[0]); // 사용법 출력
        return 1; // 프로그램 종료
    }

    char* algorithm = argv[1]; // 알고리즘 이름 가져오기
    int num_frames = atoi(argv[2]); // 프레임 수 가져오기
    PageRequest requests[MAX_ACCESSES]; // 페이지 요청 배열
    int total_requests = 0; // 총 요청(read + write) 수 초기화
    int total_reads = 0; // 총 read 수 초기화
    int total_writes = 0; // 총 write 수 초기화

    FILE* file = fopen("access.list", "r"); // 파일 열기
    
    while (fscanf(file, "%d %s", &requests[total_requests].page_id, requests[total_requests].action) != EOF) // 파일에서 데이터를 읽어옴
    {
        if (strcmp(requests[total_requests].action, "read") == 0) // 작업이 read라면
        {
            total_reads++; // read 수 증가
        }
        else if (strcmp(requests[total_requests].action, "write") == 0) // 작업이 write라면
        {
            total_writes++; // write 수 증가
        }
        total_requests++; // 총 요청(read + write) 수 증가
    }
    fclose(file); // 파일 닫기

    int page_faults;
    if (strcmp(algorithm, "fifo") == 0) // 입력받은 알고리즘이 FIFO라면
    {
        page_faults = fifoReplacement(requests, total_requests, num_frames); // FIFO 알고리즘 실행
    }
    else if (strcmp(algorithm, "nru") == 0) // 입력받은 알고리즘이 NRU라면
    {
        page_faults = nruReplacement(requests, total_requests, num_frames); // NRU 알고리즘 실행
    }

    int page_hits = total_requests - page_faults; // 페이지 히트 수 계산
    double page_fault_rate = (double)page_faults / total_requests * 100; // 페이지 폴트 비율 계산

    printf("Total number of access: %d\n", total_requests); // 총 접근 수 출력
    printf("Total number of read: %d\n", total_reads); // 총 읽기 수 출력
    printf("Total number of write: %d\n", total_writes); // 총 쓰기 수 출력
    printf("Number of page hits: %d\n", page_hits); // 페이지 히트 수 출력
    printf("Number of page faults: %d\n", page_faults); // 페이지 폴트 수 출력
    printf("Page fault rate: %d/%d = %.3f%%\n", page_faults, total_requests, page_fault_rate); // 페이지 폴트 비율 출력

    return 0; // 프로그램 종료
}
