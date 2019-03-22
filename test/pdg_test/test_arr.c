#include <stdint.h>
#include <stdio.h>
struct P {
__uint16_t queue_mapping;
__uint8_t    cloned: 1,
         nohdr: 1,
         fclone: 2,
         peeked: 1,
         head_frag: 1,
         xmit_more: 1;
};

int main() {
    struct P p = {1,1,1,1,1,1,1};
    p.fclone = 3;
    p.peeked = 1;
    return 0;
}
