#include <stdio.h>
#include <string.h>

struct person_t{
  int age;
  char name[10];
};

/* int f(struct person_t *p) { */
/*     return p->age; */
/* } */

int f1(struct person_t *p, int num) {
    return p->age + num;
}

int main() {
    int a = 5;
    struct person_t p;
    p.age = 10;
    strncpy(p.name, "Jack", 4);
  
    //int b = f(&p);
    int b = f1(&p, a);
    int c = a + b;

    printf("Age:%d", p.age);
    return 0;
}
