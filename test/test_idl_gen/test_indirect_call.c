#include "stdio.h"
#include "stdlib.h"

int Global = 10;

typedef struct list {
// struct list* Next;
int Data;
}list;

void fun2(int* X) {
    (*X) += Global - 1;
}

void fun(int* X){
    // write
    (*X)+=Global;
    
    // read 
    // if((*X)>10){
    //     printf("value less than 10");
    // }
}

 void accessF(struct list* L, void (*FP)(int*)){       
         // should report r/w's reported by function pointed by 
         //FP
         FP(&L->Data);
 }

 void passF(struct list* L){
     // should report r/w's reported by function accessF
     accessF(L,fun);
     accessF(L,fun2);
 }

int main(){
    struct list *X = malloc(sizeof(struct list));
    struct list *Y = alloca(sizeof(struct list));
    passF(X);
}
