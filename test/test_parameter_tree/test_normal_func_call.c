#include <stdio.h>

int f1(int x){
    printf("f1: %d\n",x);
    return 0;
}

int f2(int x) {
    printf("f2: %d\n",x);
    return 0;
}

int main(){
    int a = 4;
    f1(a);
    f2(a);
    return 0;
}
