#include <stdio.h>
#include <stdint.h>
#include <assert.h>

uint16_t merge_bytes(uint8_t high_byte, uint8_t low_byte);

int main(void) {
    uint16_t result = merge_bytes(0xAB, 0xCD);
    assert(result == 0xABCD);
    printf("Success! Result is %X\n", result);
    return 0;
}

uint16_t merge_bytes(uint8_t high_byte, uint8_t low_byte) {
    // 1010 1011 upper half
    // 1100 1101 lower half
    uint16_t merged = 0; 
    // switch and merge first high_byte the first 8 bytes
    // and then OR with the low_byte
    merged |= (high_byte << 0x08) | low_byte;
    printf("%hu\n", merged);
    return merged;
    
    // Here what happen during the switch
    // based on the mask we use
    //
    // switch just 1 (0x01) 
    // tar: 1010 1011 1100 1101
    // cur: 0000 0001 1101 1111
    //
    // switch just 2 (0x02)
    //
    // tar: 1010 1011 1100 1101
    // cur: 0000 0010 1110 1101
}

