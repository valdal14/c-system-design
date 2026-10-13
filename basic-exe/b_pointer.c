#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
    uint64_t *big_num = (uint64_t *)calloc(1, sizeof(uint64_t));
    if(big_num == NULL) exit(EXIT_FAILURE);
    printf("big_num value: %llu\n", *big_num);

    // alloc a new uint8_t into the big_num block 
    uint8_t *byte_lens = (uint8_t *)((char *)big_num);
    // dereference to assign a value 
    *byte_lens = 255;

    // use the pointer to move to the next offset
    byte_lens = (uint8_t *)((char *)big_num + 1);
    // add one 
    *byte_lens = 1;

    // Memory footprint
    // bite 1  - byte 2 - byte 3 - byte 4 - byte 5 - byte 7 - byte 7 - byte 8
    // 00000000-00000000-00000000-00000000-00000000-00000000-00000000-00000000
    //
    // 255 was written at byte 0 
    // 11111111-00000000-00000000-00000000-00000000-00000000-00000000-00000000
    //
    // then I move the same pointer by one byte and write 1 into byte 2 
    // 11111111-00000001-00000000-00000000-00000000-00000000-00000000-00000000
    //
    // then comes the addition
    // 1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 = 255 
    // the 9th bit does not fit move to byte 1 and 256^1 = 256 
    // 255 + 256 = 511

    printf("big_num value: %llu\n", *big_num);
    
    free(big_num);

    return 0;
}
