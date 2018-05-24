#include <stdio.h>

typedef void (*fp)(int a, int b, int c);

void f1(int a, int b, int c) {
    //printf("\n f1: a: %d, b: %d", a, b);
}

void f2(int a, int b, int c) {
    printf("\n f2: a: %d, b: %d, c: %d", a, b, c);
}

void f3(int a, int b, int c) {
    printf("\n f3: a: %d, b: %d, c: %d", a, b, c);
}

int main() {
    /* fp fp1 = (fp)f1; */
    fp fp2 = (fp)f2;
    /* fp fp3 = &f3; */

    //f1(1,2,3);
    /* fp1(1, 2, 3); */
    fp2(4, 5, 6);
    /* fp3(7, 8, 9); */

    //f2(3,4,5);
    /* f3(4,5,6); */
    return 0;
}
