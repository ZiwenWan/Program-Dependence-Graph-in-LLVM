#include <stdio.h>
int __attribute__((noinline)) f2 (int *a){
 return *a + 16 - (int)(a)|4;
}

int __attribute__((noinline)) f1 (int *a, int *b){
	printf("%d\n", *b);
	int c = f2(a) - f2(b);
	return f2(b) * f2(&c);
}

int main() {
	int a = 4;
	int b = 5;
	f1(&a, &b);
}
