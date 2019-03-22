#include <stdio.h>

int f(void* key) {
   int* i = (int*)key;
   return *i;
}

int main() {
   int a = 5;
   f((void*)&a);
}
