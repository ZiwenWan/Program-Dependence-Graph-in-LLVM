#include <stdio.h>
struct Person {
    int age;
    char *name;
};

int f(struct Person* p) {
    printf("%d\n", p->age);
    return 0;
}

int main() {
    struct Person p = {1, "J"};
    printf("%d\n", p.age);
}
