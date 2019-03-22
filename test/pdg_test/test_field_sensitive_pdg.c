#include <stdio.h>
struct Person {
    int age;
    char *name;
} Person;

int f(struct Person* p) {
    return p->age;
}

int main() {
    struct Person p = {20, "Doo"};
    f(&p);
    return 0;
}
