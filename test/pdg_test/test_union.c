#include <stdio.h>
#include <stdlib.h>

union UTEST{
    struct {
        int c;
        int b;
    } B;
    struct {
        int d;
    } C;
};

int f(union UTEST *utest) {
    if (utest->B.c > 5)
        return utest->B.c;
    return 5;
}

int main() {
    union UTEST utest;
    utest.B.c = 10;
    f(&utest);
    return 0;
}

