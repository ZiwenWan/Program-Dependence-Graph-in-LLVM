#include<stdio.h>
int f1(int a, int b) {
    return a+b;
}

int f2(int a, int b) {
    return a*b;
}

int main() {
   int a = 5;
   int b = 5;
 
   int c = 10;
   if (c < 10) {
    f1(a, b);
   } else {
    f2(a,b);   
   }

}
