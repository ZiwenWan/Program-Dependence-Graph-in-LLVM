#include <stdio.h>

struct S1 {
    int a;
    float b;
};

void print(struct S1 *p) {
    printf("%d\n",p->a);
}

int main() {
   struct S1 *s1;
   s1->a = 5;
   s1->b = 1.33f;
   print(s1);
   return 0;
}
