#include<stdio.h>

struct struct_t{
    int a;
    float b;
};

int getFlag() {
    return 1;
}
    
int main() {
    struct struct_t s;
    int a = 5;
    float b = 6.0;

    if (getFlag()) {
        s.a = a;
    } else {
        s.b = b;
    }


    return 0;
}
