#include <stdio.h>

int f2(int* a) {
    return *a;
}

int f1(int* a) {
    return f2(a);
}

int main() {
 int a = 5;
 f1(&a);
}
