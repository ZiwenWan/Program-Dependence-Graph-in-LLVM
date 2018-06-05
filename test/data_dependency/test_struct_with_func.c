#include <stdio.h>

typedef struct{
 int x;
 int y;
}S2;

typedef struct{
 float f;
 S2 *sp;
}S1;

void func(S1 *p1, S2 *p2) {
    printf("%d\n",p1->sp->x);
    printf("%d\n", p2->x);
}

int main() {
    S1 s1;
    S2 s2;
    func(&s1,&s2);
    return 0;
}
