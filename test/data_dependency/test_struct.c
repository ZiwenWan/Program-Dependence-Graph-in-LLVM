#include<stdio.h>
#include<string.h>

struct person_t{
    int age;
    char name[10];
};

int main() {
    struct person_t p1;
    p1.age = 13;
    strncpy(p1.name, "foo", 5);

    printf("p1 age is:%d\n", p1.age);
    return 0;
}
