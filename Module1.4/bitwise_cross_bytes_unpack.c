#include <stdio.h>
#include <stdint.h>

typedef struct DualAdcReading
{
    // 0000 0000 0000 0000 
    uint16_t sensor_a;
    uint16_t sensor_b;
} DualAdcReading;

DualAdcReading unpack_12bit_sensors(const uint8_t *raw);

int main(void)
{
    uint8_t raw[3] = {25, 15, 14};
    DualAdcReading dcr = unpack_12bit_sensors(raw);
    printf("DCR's sensor_a: %d\n", dcr.sensor_a);
    printf("DCR's sensor_b: %d\n", dcr.sensor_b);
    return 0;
}

DualAdcReading unpack_12bit_sensors(const uint8_t *raw) {
    
    uint8_t *p1 = (uint8_t *)raw + (0 * sizeof(uint8_t));
    uint8_t *p2 = (uint8_t *)raw + (1 * sizeof(uint8_t));
    uint8_t *p3 = (uint8_t *)raw + (2 * sizeof(uint8_t));
    uint8_t v1 = *p1;
    uint8_t v2 = *p2;
    uint8_t v3 = *p3;
    
    v1 |= (v1 << 0x04) & (v1 >> 0x04);
    v2 |= (v2 << 0x04) & (v2 >> 0x04);

    DualAdcReading dcr;

    dcr.sensor_a |= (v1 & (v2 >> 4) & 0x04);
    dcr.sensor_b |= (v2 & (v3 >> 4) & 0x04);
    return dcr;
}
