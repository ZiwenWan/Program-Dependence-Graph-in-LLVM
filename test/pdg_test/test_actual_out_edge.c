#include <stdio.h>
typedef struct Person {
    int age;
    char *name;
} person_t;

void f(int a) {
    printf("%d\n", a);
}

int main() {
    person_t *p1;
    struct Person pp = {15, "Jack"};
    p1 = &pp;
    int age = p1->age;
    f(age);
    int age1 = p1->age; 
    f(age1);
    int age2 = p1->age;

    return 1;
}
