#include <stdio.h>
#include <stdint.h>
#include <assert.h>

int main(void) {
    uint8_t stream[2] = { 0xAB, 0xCD }; 
    
    // (Notice we cast stream[0] to uint16_t first so it has a 16-bit box to shift inside of!)
    uint16_t sensor_id = (uint16_t)((stream[0] << 8) | stream[1]);
    
    printf("Parsed Sensor ID is: %X\n", sensor_id);
    assert(sensor_id == 0xABCD);
    
    return 0;
}
