#include<stdio.h>

typedef struct clothes {
    int length;
    char *color;
} clothes_t;

typedef struct Person {
    int age;
    clothes_t* c; 
} person_t;

int f(person_t* p) {
    printf("%s\n", p->c->color);
    return p->age;
}

int main() {
    clothes_t c = {10, "red"};
    person_t p = {21, &c};
    f(&p);
}
