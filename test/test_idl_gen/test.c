#include <stdio.h>
#include <string.h>

typedef struct {
    int age;
    char name[10];
} Person;

void func1(char *name) {
    printf("%s", name);
    strncpy(name, "new_name", 10);
}

void func2(int* age) {
    age += 1;
    /* int* tmp_age = age; */
    /* printf("%d\n", *tmp_age); */
}

void func3(int age) {
    printf("%d\n", age);
}

int main() {
    Person p = { 15, "Jack" };
    func2(&p.age);
    func1(p.name);
    func3(p.age);
}
