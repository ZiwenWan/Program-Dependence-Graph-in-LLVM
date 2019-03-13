#include <stdio.h>
struct Person {
    int age;
    char *name;
};

int k(struct Person** p) {
    return 0;
}

int f(struct Person* p) {
    /* struct Person p1 = *p; */
    /* printf("%d\n", p1.age); */
    k(&p);
    return 0;
}

int main() {
    struct Person p = {1, "J"};
    printf("%d\n", p.age);
    f(&p);
}
