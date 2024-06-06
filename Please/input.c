#include<stdio.h>

int main() {
    float realNum = 0;
    int integer = 0;
    char character = 0;

    scanf("%d %f %c", &integer, &realNum, &character);

    printf("%d\n%.2f\n%c\n", integer, realNum, character);

    return 0;
}