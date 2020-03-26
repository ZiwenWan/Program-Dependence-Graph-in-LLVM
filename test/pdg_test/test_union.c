#include <stdio.h>
#include <stdlib.h>

struct B {
    int c;
};

struct C {
    int d;
};

struct S {
    int a;
    union {
        struct B* b;
        struct C* c;
    };
};

int main() {
    struct B b = {5};
    struct C c = {10};
    struct S* ss = (struct S*)malloc(sizeof(struct S));
    ss->a = 5;
    ss->b = &b;
    ss->c = &c;
    printf("%d\n", ss->b->c);
    printf("%d\n", ss->c->d);
    free(ss);
    return 0;
}
