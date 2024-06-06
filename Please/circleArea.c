#include<stdio.h>

float circleArea(int r, float pi);

int main() {
    int radius = 0;
    const float PI = 3.14;
    float cirArea = 0;

    printf("Type the radius : ");
    scanf("%d", &radius);

    cirArea = circleArea(radius, PI);
    printf("%.2f", cirArea);

    return 0;
}

float circleArea(int r, float pi){
    return r*r*pi;
}