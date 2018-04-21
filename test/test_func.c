#include <stdio.h>

int add(int a, int b) {
    return a + b;
}

int main() {
    int a = 5;
    int b = 3;
    int c = add(a,b);
    printf("Value of c is %d\n", c);
    return 0;
}
