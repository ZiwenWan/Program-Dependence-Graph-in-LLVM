#include <stdio.h>
#include <stdlib.h>

void use1(int *addr) {
	printf("%d\n", *addr);
}

void use2(int *addr) {
	printf("%d\n", addr[1]);
}

void multiuse(int *a1, int *a2) {
    use1(a1);
    use2(a2);
}

void f(int a, float b) {
    printf("%d + %f", a, b);
}

int main () {
	int *heap;
	int local1 = 7;
	int local2 = 8;
    float flt = 4.333;


    f(local1, flt);
	heap = (int*)malloc(16);
	multiuse(&local2, &local1);
	multiuse(heap, &local1);
}
