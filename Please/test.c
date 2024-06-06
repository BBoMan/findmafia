#include <sys/syscall.h>
#include <stdio.h>

int main() {
    // 452번 시스템 호출을 수행합니다.
    syscall(452);

    return 0;
}
