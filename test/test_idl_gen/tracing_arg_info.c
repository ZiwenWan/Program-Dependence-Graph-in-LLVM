#include <stdio.h>
#include <string.h>

struct person_t {
    int age;
    char name[10];
};

typedef struct person_t Person;

void f1(char *name) {
    strncpy(name, "new_name", 10);
    printf("New name: %s\n", name);
}

void f(Person *p) {
    printf("current name %s\n", p->name);
    f1(p->name);
}

int main() {
    Person p1 = { 14, "Foo" };
    f(&p1);
    return 0;
}
