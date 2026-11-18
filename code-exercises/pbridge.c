#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef struct {
    uint64_t a;
    uint32_t b;
    uint16_t c;
    uint8_t d;
} CompactPacket;

int main(void) {
    CompactPacket array[2];
    
    // Initialize the second packet's 'c' property to a specific value
    array[1].c = 4096;
    
    // The raw byte pointer you are forced to use
    uint8_t *ptr = (uint8_t *)&array[0];
    
    /**
     * To get to uint16_t c value, we need to move ahead by the sizeof of the CompactPacket
     * since the array[0] contains the firsy struct + the size of the first two elemenets 
     * a + b = 12 
     */
    uint16_t extracted_c = *((uint16_t *)((char *)ptr + sizeof(CompactPacket) + 12));
    printf("expected: %hu\n", extracted_c);
    
    assert(extracted_c == 4096);
    printf("Drill 1.2: PASSED\n");
    return 0;
}
