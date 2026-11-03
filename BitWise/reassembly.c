#include <stdio.h>
#include <stdint.h>
#include <assert.h>

int main(void) {
    uint8_t top_half = 0x05;    // 0000 0101
    uint8_t bottom_half = 0x0C; // 0000 1100
    
    /**
     * Moved the bits 4 slots to the right first
     * 0101 0000 
     * The OR with 0000 1100 to glue them together
     * 0101 0000 |
     * 0000 1100 =
     * ---------
     * 0101 1100 
     */
    uint8_t reassembled_byte = (top_half << 4) | bottom_half;
    
    printf("Reassembled byte is: %X\n", reassembled_byte);
    assert(reassembled_byte == 0x5C);
    
    return 0;
}
