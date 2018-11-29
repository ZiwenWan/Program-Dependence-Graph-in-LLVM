#include <stdio.h>
#include <stdlib.h>

int main() {
    char *name;
    name = (char*) malloc(10); 
    printf("%s\n", name);    

    return 0;
}
