#include <assert.h>
#include <stdint.h>
#include <stdio.h>

// mask to compare the first most right bits in 1 byte  
#define FOUR_BIT_MASK 0x0F

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
 * @brief Decodes a 16-bit compressed telemetry frame into its constituent parts.
 *
 * @param raw_frame The 16-bit integer received from the network.
 * @return A TelemetryPacket struct containing the unpacked values.
 */
TelemetryPacket decode_telemetry(uint16_t raw_frame);

int main(void)
{
    uint16_t incoming_packet = 1223; // 00000100-11000111
    TelemetryPacket tp = decode_telemetry(incoming_packet);
    assert(tp.sensor_id == 4);
    assert(tp.error_code == 12);
    assert(tp.priority == 7);
    return 0;
}

TelemetryPacket decode_telemetry(uint16_t raw_frame) {

    // shift 8 bits and cast into uint8_t 
    uint8_t sensor_id = (uint8_t)(raw_frame >> 8);
    printf("sensor_id decoded value : %d\n", sensor_id);
    // cast to 8 bits to get the second part of the 16 bits 11000111
    uint8_t second_half = (uint8_t)raw_frame;
    // once we got the second half we AND with the FOUR_BIT_MASK (0x0F)
    uint8_t priority = second_half & FOUR_BIT_MASK;
    printf("priority decoded value  : %d\n", priority);
    // for the error_code that sits at the second part of the seond byte 1100
    // we shift them to the right by 4 spaces and cast it to uint8_t and 
    // use the same mask 0x0F
    uint8_t error_code = ((second_half >> 4) & FOUR_BIT_MASK);
    printf("error_code decoded value: %d\n", error_code);
    
    
    TelemetryPacket decode_packet;
    decode_packet.sensor_id = sensor_id;
    decode_packet.error_code = error_code;
    decode_packet.priority = priority;

    return decode_packet;
}
