#include <stdio.h>
int f(int* a) {
    int *b = a;
    return *b;
}

int main() {
    int a =5;
    int b = f(&a);
    return 0;
}
