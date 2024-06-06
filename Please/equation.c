#include<stdio.h>

double equation(int a, int b, int x);

int main() {
    int a, b, x =0;
    double result = 0;

    printf("a와 b 와 x를 입력하세요 : ");
    scanf("%d %d %d", &a, &b, &x);

    result = equation(a, b, x);

    printf("다항식의 결과는 %.2lf입니다.", result);
    
    return 0;
}

double equation(int a, int b, int x) {
    return (a*x*x*x - b*x*x + a*x - b)*(a + b);
}