#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define PK_LEN 8
#define PK_NUM 4
#define BIT(x) (1 << (x))
#define FLAG_ACTIVE_BIT 0
#define FLAG_ERROR_BIT 2 

struct TelemetrySorter;

typedef void (*Sorter)(struct TelemetrySorter *self, size_t size, int(*compare)(const void *a, const void *b));

typedef struct TelemetrySorter
{
    struct TelemetryPacket **packets;
    Sorter exec;
} TelemetrySorter;

typedef struct TelemetryPacket
{
    uint32_t payload; 
    uint16_t sensor_id; 
    uint8_t status_flags; 
} TelemetryPacket;

typedef struct AllocationBuffer
{
    void **buffer;
    int8_t return_value;
} AllocationBuffer;

typedef struct ParsedPacket
{
    TelemetryPacket packet;
    int8_t return_value;
} ParsedPacket;

/**
 * @brief Allocs a buffer represented by a 2D array
 *
 * @param buffer void double pointer
 * @param num_of_packets size_t
 * @return AllocationBuffer. If the AllocationBuffer return_value is != 0 
 * means the allocation of the buffer failed.
 */
AllocationBuffer alloc(size_t num_of_packets);

/**
 * @brief Parses the stream and return a ParsedPacket that contais a single
 * TelemetryPacket and the return_value
 *
 * @param stream uint8_t pointer
 * @param index size_t
 * @return ParsedPacket struct 
 */
ParsedPacket parse(uint8_t *stream, size_t index);

/**
 * @brief Sorts processed and stored packets
 *
 * @param a const void pointer
 * @param b const void pointer
 * @return int
 */
int compare(const void *a, const void *b);

/**
 * @brief Sorts the TelemetryPacket by sensor_id using qsort standard library algorithm.
 *
 * @param self struct TelemetryPacket that contais the pointer to the 2D buffer
 * @param size size_t
 * @param compare callback for the qsort
 * @return void
 */
void sort_by(struct TelemetrySorter *self, size_t size, int(*compare)(const void *a, const void *b));

/**
 * @printf Prints the values stored in the packets stored in the 2D buffer 
 *
 * @param buffer void double pointer
 * @param size size_t
 * @return void
 */
void print_parsed_packets(void **buffer, size_t size);

/**
 * @brief Frees the allocated memory
 *
 * @param buffer void double pointer
 * @return void
 */
void cleanup(void **buffer);

int main(void)
{
    void **buffer = NULL;
    AllocationBuffer new_buffer = alloc(PK_NUM);

    if (new_buffer.return_value == 0 && new_buffer.buffer != NULL) 
    {
        buffer = new_buffer.buffer;
        printf("Buffer allocated at address: %p\n", (void *)buffer);
    }
    else 
    {
        exit(EXIT_FAILURE);
    }

    // UDP network protocol -> Network Byte Order (Big-Endian)
    uint8_t mock_stream[32] = {
        // Packet 1: Valid & Standard (Status: 0x01)
        0x01, 0x00, 0x00, 0x65, 0x00, 0x00, 0x11, 0x5C,
        // Packet 2: Error - Must Drop (Status: 0x01 | 0x04 = 0x05)
        0x05, 0x00, 0x00, 0x66, 0x00, 0x00, 0x27, 0x0F,
        // Packet 3: Valid & Critical (Status: 0x01 | 0x08 = 0x09)
        0x09, 0x00, 0x00, 0x67, 0x00, 0x00, 0x1E, 0x61,
        // Packet 4: Inactive - Must Drop (Status: 0x00)
        0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x04, 0x57
    };
   
    // start the packet parsing
    ParsedPacket pp;
    uint8_t counter = 0;

    for (size_t i = 0; i < sizeof(mock_stream); i += PK_LEN) 
    {
        printf("Start parsing packet at index: %zu\n", i);

        pp = parse(mock_stream, i);
        
        if (pp.return_value != 0) {
            printf("Corrupted bit at index[%zu]. The packet will be dropped\n", i);
        } else {
            printf("Return value: %d\n", pp.return_value);
            memcpy(*(buffer + counter), &pp.packet, sizeof(TelemetryPacket));
            counter += 1;
        }
    }

    // print before sorting 
    print_parsed_packets(buffer, counter);
    
    TelemetrySorter ts;
    ts.exec = sort_by;
    ts.packets = (TelemetryPacket **)buffer;
    // execute the sorting 
    ts.exec(&ts, counter, compare);

    // after sorting 
    print_parsed_packets(buffer, counter);

    // deallocate memory
    cleanup(buffer);

    return 0;
}

 AllocationBuffer alloc(size_t num_of_packets) {

    void **telemetry_buffer = (void **)calloc(num_of_packets, sizeof(TelemetryPacket *));
    AllocationBuffer alloc_buffer;

    if (telemetry_buffer == NULL || num_of_packets <= 0) {
        alloc_buffer.buffer = NULL;
        alloc_buffer.return_value = 1;
        return alloc_buffer;
    }

    for (size_t i = 0; i < num_of_packets; i++) {
        *(telemetry_buffer + i) = calloc(1, sizeof(TelemetryPacket));

        if (*(telemetry_buffer + i) == NULL) {
            // rollback, free and return 
            for (size_t j = 0; j < i; j++) {
                if (*(telemetry_buffer + j) != NULL) free(*(telemetry_buffer + j));
            }

            free(telemetry_buffer);
            alloc_buffer.buffer = NULL;
            alloc_buffer.return_value = 1;
            return alloc_buffer;
        }
    }

    alloc_buffer.buffer = telemetry_buffer;
    alloc_buffer.return_value = 0;
    return alloc_buffer;
}


ParsedPacket parse(uint8_t *stream, size_t index) {

    ParsedPacket parsed; 

    if (stream == NULL)
    {
        parsed.return_value = 1;
        return parsed;
    }

    // Byte 0 of this chunk is the status
    uint8_t status = stream[index + 0];
    // Bytes 2 and 3 of this chunk are the ID (Network Byte Order)
    uint16_t id = ((uint16_t)stream[index + 2] << PK_LEN) | (uint16_t)stream[index + 3];
    // Bytes 4, 5, 6, 7 are the payload
    uint32_t payload = ((uint32_t)stream[index + 4] << (PK_LEN * 3)) |
        ((uint32_t)stream[index + 5] << (PK_LEN * 2)) |
        ((uint32_t)stream[index + 6] << PK_LEN)  |
        ((uint32_t)stream[index + 7]);
        
    if ((status & BIT(FLAG_ERROR_BIT)) != 0 || (status & BIT(FLAG_ACTIVE_BIT)) == 0) 
    {
        parsed.return_value = 1;
        return parsed; 
    } 
    else 
    {
        TelemetryPacket packet;
        packet.payload = payload;
        packet.sensor_id = id;
        packet.status_flags = status;
            
        parsed.packet = packet;
        parsed.return_value = 0;
    }
    
    return parsed;
}

void sort_by(struct TelemetrySorter *self, size_t size, int(*compare)(const void *a, const void *b)) {
    qsort(self->packets, size, sizeof(TelemetryPacket *), compare);   
}

int compare(const void *a, const void *b) {
    TelemetryPacket *pa = *(TelemetryPacket **)a;
    TelemetryPacket *pb = *(TelemetryPacket **)b;
    return (int)pa->sensor_id < (int)pb->sensor_id; 
}


void print_parsed_packets(void **buffer, size_t size) {
    
    if (buffer == NULL) return;

    for (size_t i = 0; i < size; i++) {
        TelemetryPacket *tp = *(buffer + i);
        printf("At index[%zu] is stored the packet with sensor id: %d\n", i, tp->sensor_id);
    }
}

void cleanup(void **buffer) {
    if (buffer == NULL) return;
    
    for (size_t i = 0; i < PK_NUM; i++)
    {
        if(*(buffer + i) != NULL) free(*(buffer + i));
    }

    free(buffer);
}
