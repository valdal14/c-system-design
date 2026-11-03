#include <stdio.h>
#include <stdint.h>
#include <assert.h>

int main(void) {
    uint8_t shared_byte = 0x5C; // 0101 1100
                                
    /**
     * Getting back the first 4 bits from the left
     * starting from the right and selecting 4 bits and moving right 4 places
     * to get back 0101 = 5
     * 0xF means 4 bits (1111) max value 15
     * 0x7 means 3 bits (0111) max value 7
     */
    uint8_t top_half = (shared_byte >> 4) & 0xF;
  
    /**
     * The bottom_half is already in place we 
     * just need to erase the first 4 bits on 
     * the left using the & operator
     *
     * 0101 1100 &
     * 0000 1100 =
     * ---------
     * 0000 1100 
     */
    uint8_t bottom_half = (shared_byte & 0xF);

    printf("Top half is: %X\n", top_half);
    printf("Bottom half is: %X\n", bottom_half);
    
    assert(top_half == 0x05);
    assert(bottom_half == 0x0C);
    
    return 0;
}
