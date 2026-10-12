#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint32_t sensor_id;
    uint32_t temperature;
} TelemetryPacket; // 8 bytes total

int main(void) {
    // Simulating a raw network buffer containing 3 packets (24 bytes total)
    void *network_buffer = calloc(3, sizeof(TelemetryPacket));
    if (network_buffer == NULL) return 1;

    // 1. Pointer Arithmetic Approach 
    TelemetryPacket *packet_1 = (TelemetryPacket *)network_buffer;
    packet_1->sensor_id = 100;
    
    TelemetryPacket *packet_2 = (TelemetryPacket *)((char *)network_buffer + 8);
    packet_2->sensor_id = 101;

    TelemetryPacket *packet_3 = (TelemetryPacket *)((char *)network_buffer + 16);
    packet_3->sensor_id = 102;

    printf("Packet 1 ID: %d\n", packet_1->sensor_id);
    printf("Packet 2 ID: %d\n", packet_2->sensor_id);
    printf("Packet 3 ID: %d\n", packet_3->sensor_id);

    // Array index approach 
    TelemetryPacket *p1 = (TelemetryPacket *)&((char *)network_buffer)[8];
    p1->sensor_id = 200;
   
    TelemetryPacket *p2 = (TelemetryPacket *)&((char *)network_buffer)[8];
    p2->sensor_id = 201;
    
    TelemetryPacket *p3 = (TelemetryPacket *)&((char *)network_buffer)[16];
    p3->sensor_id = 201;
    
    printf("Packet 1 ID: %d\n", p1->sensor_id);
    printf("Packet 2 ID: %d\n", p3->sensor_id);
    printf("Packet 3 ID: %d\n", p3->sensor_id);

    // Create a TelemetryPacket in the stack 
    TelemetryPacket stack_p = *(TelemetryPacket *)network_buffer;
    stack_p.sensor_id = 300;
    printf("Packet 1 ID: %d\n", stack_p.sensor_id);
    // should be 200
    printf("Packet 1 ID: %d\n", p1->sensor_id);

    // 3. Cleanup
    free(network_buffer);

    return 0;
}
