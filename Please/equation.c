#include<stdio.h>

double equation(int a, int b, int x);

int main() {
    int a, b, x =0;
    double result = 0;

    printf("a�� b �� x�� �Է��ϼ��� : ");
    scanf("%d %d %d", &a, &b, &x);

    result = equation(a, b, x);

    printf("���׽��� ����� %.2lf�Դϴ�.", result);
    
    return 0;
}

double equation(int a, int b, int x) {
    return (a*x*x*x - b*x*x + a*x - b)*(a + b);
}