#include <stdio.h>

typedef  struct {
    int age;
    char name[10];
} P;


int f(P *a, P *b) {
    return a->age + b->age;
}

int main() {
    P a;
    a.age = 10;
    P b;
    b.age = 10;

    printf("%d\n", f(&a,&b));

    return 0;
}
