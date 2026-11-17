#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

typedef struct 
{
    // 1 bit 
    uint8_t a;
    // 8 bytes 
    uint64_t b;
    // 4 bytes 
    uint16_t c;
    // 2 bytes 
    uint32_t d;
    /**
     * a -> start offset 0 and next offset 1 
     * b -> temp: (1 + 8) - 16 = 7 
     * b -> start offset 9 next offset 16 
     * c -> start offset 17 and next 20 
     * d -> start offset 21 and and offset 22
     *
     * The struct size should be 22 + 2 paddings 
     *
     * Law II — Struct-Level Alignment ---
     * 
     * max size (1, 8, 4, 2) = 8
     * 24 % 8 = 0 
     *
     * Law III — Tail Padding ------------
     *
     * last struct member is d;
     * sizeof(struct NaivePacket) - (offset(d) + sizeof(d))
     * 24 - (22 + 2) = 0 
     *
     */
} NaivePacket;

typedef struct
{
    // 8 bytes 
    uint64_t a;
    // 4 bytes 
    uint32_t b;
    // 2 bytes 
    uint16_t c;
    // 1 byte 
    uint8_t d;
    /**
     * a -> start offset 0 and next offset 8
     * b -> start offset 9 and next offset 12
     * c -> start offset 13 and next offset 14
     * d -> start offset 15
     *
     * The struct size should be 15 + 1 padding
     *
     * Law II — Struct-Level Alignment ---
     * 
     * max size (1, 8, 4, 2) = 8
     * 16 % 8 = 0 
     *
     * Law III — Tail Padding ------------
     *
     * last struct member is d;
     * sizeof(struct NaivePacket) - (offset(d) + sizeof(d))
     * 16 - (15 + 1) = 0 
     */
} CompactPacket;

int main(void)
{
    NaivePacket np;
    CompactPacket cp;
    assert(sizeof(np) == 24);
    assert(sizeof(cp) == 16);
    // saving 7 bytes for each struct allocated
    assert((uint8_t)(sizeof(np) - sizeof(cp)) == 8);
    return 0;
}
