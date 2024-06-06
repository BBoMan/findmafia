#include<stdio.h>

int main() {
    char character = 0;
    float realNum = 0;
    //printf 안에서 %f는 형변화 과정에서 오류가 발생한다. (0출력)
    scanf("%c", &character);
    realNum = character;

    printf("%d\n%c\n%.2f", character, character, realNum);

    return 0;
}