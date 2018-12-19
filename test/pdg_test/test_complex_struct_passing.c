#include <stdio.h>

typedef struct clothes {
    char color[10];
    int length;
} Clothes;

typedef struct person_t {
    int age;
    char name[10];
    Clothes *s;
} Person;

void f(Person *p1) {
    char *name = p1->name;
    char *color = p1->s->color;
    printf("%s is wearing %s today.", name, color);
}

int main() {
    Clothes c = {"red", 5};
    Person p = {10, "Jack", &c};
    Person *pt = &p;
    f(pt);
    return 0;
}
