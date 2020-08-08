#include <stdio.h>

struct C {
  int a;
  int b;
};

int* f(struct C* c) {
    return &(c->a);
}

int main() {
    int a = 5;
    struct C c = {a, 3};
    int *b = f(&c);
    return 0;
}
