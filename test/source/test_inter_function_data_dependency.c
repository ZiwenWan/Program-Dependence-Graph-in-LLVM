#include<stdio.h>

void f2(int a, float b) {
    printf("int: %d, float: %f\n", a, b);
}

void f1(int a, float b) {
   f2(a,b); 
}

int main() {
    int a = 5;
    float b=  5.5;
    f1(a,b);
    return 0;
}
