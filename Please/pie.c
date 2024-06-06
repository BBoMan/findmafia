#include<stdio.h>

double circleArea(int r, double pi);

int main(){
    int radius = 0;
    const double PI = 3.14;
    double circumference = 0; 

    printf("Type the radius : ");
    scanf("%d", &radius);
    //circumference = 2*radius*PI;

    circumference = circleArea(radius, PI);

    printf("%lf", circumference);
    

    return 0;

}

double circleArea(int r, double pi){
    return 2*pi*r;
}