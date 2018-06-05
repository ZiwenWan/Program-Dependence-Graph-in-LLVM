#include <stdio.h>
int main() {
    int a = 0;
    int* b = &a;
    int c = a + 3;
    printf("%d\n", *b);
    return 0;
}
