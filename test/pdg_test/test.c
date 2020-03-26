#include <stdio.h>
struct S {
    int a;
    int b;
};

void f(struct S *s) {
    printf("%d\n", s->a);
}

int main() {
    return 0;
}
