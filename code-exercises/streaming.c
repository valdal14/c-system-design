#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include<unistd.h>

#define MAX_RECORDS_STORED 3

typedef struct {
    // 4 bytes 
    uint16_t message_type;
    uint16_t payload_count;
} StreamHeader;

typedef struct {
    // 8 bytes 
    uint32_t record_id;
    uint32_t timestamp;
} PayloadRecord;

typedef struct {
    StreamHeader packet_header;
    PayloadRecord records[MAX_RECORDS_STORED];
} NetworPacket;

/**
 * @brief Allocates a block on memory 
 * @param void memory_block double pointer
 * @param msn_type uint16_t 
 * @return void
 */
void alloc(void **memory_block);

/**
 * @brief Properly stores the upcoming header and packets in the 
 * allocated memory block
 * @param buffer void pointer to the memory block
 * @param on_complition void pointer callback  
 * @return void
 */
void stream_packets(void *buffer, void(*on_complition)(void *buffer, uint16_t offsets[MAX_RECORDS_STORED]));

/**
 * @brief Prints out the header and packets processed from the memory 
 * block
 * @param buffer void pointer to the memory block
 * @param offsets uint16_t[MAX_RECORDS_STORED]
 * @return void
 */
void show_packets(void *buffer, uint16_t offsets[MAX_RECORDS_STORED]);

/**
 * @brief Cleans up the allocated memory block
 * @param buffer void pointer to the memory block
 * @return void
 */
void clean(void *buffer);

// MAIN METHOD ---------------------------------------------------------------------------------------------

int main(void) {
    void *memory_buffer = NULL;
    // allocate 
    alloc(&memory_buffer);
    printf("Buffer allocated at address: %p\n", memory_buffer);
    // process and print 
    stream_packets(memory_buffer, show_packets);
    // cleanup 
    clean(memory_buffer);
    return 0;
}

// IMPLEMENTATION  -------------------------------------------------------------------------------------------

void alloc(void **memory_block) {
    void *buffer = (void *)calloc(1, sizeof(NetworPacket));
    
    if (buffer == NULL) {
        fprintf(stderr, "Could not allocate space for the buffer");
        exit(EXIT_FAILURE);
    }

    *memory_block = buffer;
}

void stream_packets(void *buffer, void(*on_complition)(void *buffer, uint16_t offsets[MAX_RECORDS_STORED])) {
    
    uint16_t offsets[MAX_RECORDS_STORED];
    printf(".:: Processing and storing the packets ::.\n");
    sleep(1);

    StreamHeader *header = (StreamHeader *)((char *)buffer + 0);
    header->message_type = 14;
    header->payload_count = MAX_RECORDS_STORED;

    for (uint16_t i = 0; i < MAX_RECORDS_STORED; i++) {
        uint16_t offset = sizeof(StreamHeader) + (i * sizeof(PayloadRecord));
        offsets[i] = offset;
        PayloadRecord *record = (PayloadRecord *)((char *)buffer + offset);
        record->record_id = i + 1;
        record->timestamp = 20260910 + i;
    }

    printf("Processing completed, starting the information retrieval process\n");
    sleep(1);
    
    on_complition(buffer, offsets);
}


void show_packets(void *buffer, uint16_t offsets[MAX_RECORDS_STORED]) {
    
    printf(".:: Payload Information ::.\n");
    sleep(1);
    // retrieve the header 
    StreamHeader *h = (StreamHeader *)((char *)buffer);
    printf("Header message: %d\n", h->message_type);
    printf("Header payload_count: %d\n", h->payload_count);
    printf("------------------------------------\n");
    sleep(1);
    
    for (uint16_t i = 0; i < MAX_RECORDS_STORED; i++) {
        PayloadRecord *record = (PayloadRecord *)((char *)buffer + offsets[i]);
        printf("Record id: %d\n", record->record_id);
        printf("Record timestamp: %d\n", record->timestamp);
        sleep(1);
    }
    
}

void clean(void *buffer) {
    if (buffer != NULL) {
        free(buffer);
    }
}
