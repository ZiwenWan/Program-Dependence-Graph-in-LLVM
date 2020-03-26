#include <stdio.h>
struct A {
    void (*setup)(int*);
    int a;
};

void f1(int* b) {
    printf("%d\n", *b);
}

int f(struct A *a) {
    a->setup = &f1;
    (*a->setup)(&a->a);
    return 0;
}


int main() {

}
