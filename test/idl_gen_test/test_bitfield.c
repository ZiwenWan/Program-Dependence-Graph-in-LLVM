#include <stdint.h>

struct pack {
    __uint16_t a : 4;
    __uint8_t b : 1,
            c : 2,
            d : 3,
            e : 1;
};

int main() {
    struct pack p = {
        3, 1, 2, 3,1
    };
    p.c = 3;
    return 0;
}
