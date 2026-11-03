#include <stdio.h>
#include <stdint.h>
#include <assert.h>

typedef struct DualAdcReading {
    uint16_t sensor_a;
    uint16_t sensor_b;
} DualAdcReading;

DualAdcReading unpack_12bit_sensors(const uint8_t *raw);

int main(void) {
    uint8_t raw[3] = { 0xAB, 0xCD, 0xEF };
    
    DualAdcReading reading = unpack_12bit_sensors(raw);
    
    printf("Sensor A: %X\n", reading.sensor_a);
    printf("Sensor B: %X\n", reading.sensor_b);
    
    assert(reading.sensor_a == 0x0ABC);
    assert(reading.sensor_b == 0x0DEF);
    
    return 0;
}

DualAdcReading unpack_12bit_sensors(const uint8_t *raw) {
    DualAdcReading dcr;
    // 0xCD: 1100 1101 = 8 bits 
    // 0xC : 1100      = 4 bits
    // 0xD : 1101      = b bits 

    // TODO: Implement Part 1 (Sensor A) 0xABC
    // 0xABC = 1010 1011 1100
    // 0xAB  = 1010 1011 ----
    // 0xC   = ---- ---- 1100
    dcr.sensor_a = ((uint16_t)(raw[0] << 4) | ((raw[1] >> 4) & 0xF));
    // TODO: Implement Part 2 (Sensor B) 0xDEF
    // 0xDEF = 1101 1110 1111
    // 0xEF  = ---- 1110 1111
    // 0xD   = 1101 ---- ----
    //
    // extract the niddle D first from 0xCD and move it 8 spots 
    // and OR with the value of raw[2] that's already in the right spot 
    dcr.sensor_b = (((uint16_t)raw[1] & 0x0F) << 8) | (raw[2]);
    return dcr;
}
