#include <stdio.h>

typedef struct{
 int x;
 int y;
}S2;

typedef struct{
 float f;
 S2 *sp;
}S1;

void func(S1 *p) {
    printf("%d\n",p->sp->x);
}

int main() {
    S1 s1;
    S2 s2;
    func(&s1);
    return 0;
}
