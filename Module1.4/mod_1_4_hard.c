#include <stdint.h>
#include <stdio.h>
#include <assert.h>

#define HEX(x) printf("Encoded Hex value: 0x%02X\n", x)
#define DEC(x) printf("Encoded Dec value: %d\n", x)

/**
 * @brief Represents the unpacked telemetry data from an IoT sensor.
 */
typedef struct {
    uint8_t sensor_id;   // Extracted from bits 8-15
    uint8_t error_code;  // Extracted from bits 4-7
    uint8_t priority;    // Extracted from bits 0-3
    //
    // Law 2
    //
    // 3 bytes in total and max size 1 
    // 3 mod 1 = 0 
    //
    // Law 3 
    //
    // sizeof(TelemetryPacket) = (offsetof(priority) + sizeof(priority))
    // 3 - (2 + 1) = 0 
} TelemetryPacket; 

/**
 * @brief Encodes telemetry struct data into a 16-bit compressed network frame.
 *
 * @param packet The populated TelemetryPacket struct.
 * @return A 16-bit integer containing the packed data.
 */
uint16_t encode_telemetry(TelemetryPacket packet);

int main(void)
{
    TelemetryPacket tp;
    tp.sensor_id = 4;
    tp.error_code = 12;
    tp.priority = 7;
    // 00000100-11000111 
    uint16_t encode_tp = encode_telemetry(tp);
    assert(encode_tp == 0x04C7 && encode_tp == 1223);
    DEC(encode_tp);
    HEX(encode_tp);
    return 0;
}

uint16_t encode_telemetry(TelemetryPacket packet) {
    
    uint16_t frame = 0;
    frame |= packet.sensor_id << 8;
    frame |= packet.error_code << 4;
    frame |= packet.priority;

    return frame;
}
