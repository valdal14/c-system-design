#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_RECORDS 3

typedef struct {
    // 8 bytes
    uint64_t id;
    // 4 bytes  
    uint32_t flags;
    // 1 byte 
    uint8_t code;
    // 3 paddings
    uint8_t paddings[3];
} Record;

int main(void) {
    uint64_t ids[MAX_RECORDS] = {11110000007, 11110000014, 11110000021};
    uint32_t flags[MAX_RECORDS] = {20261007, 20261014, 20261021};
    uint8_t codes[MAX_RECORDS] = {1,2,3};

    size_t record_capacity = sizeof(Record);
    // 48 bytes block 
    size_t men_block_capacity = record_capacity * MAX_RECORDS;

    void *block = (void *)calloc(1, men_block_capacity);
    if (block == NULL) exit(EXIT_FAILURE);
    
    for (uint8_t i = 0; i < MAX_RECORDS; i++) {
        // get the address for each Record
        Record *rec = (Record *)((char *)block + (i * record_capacity));
        // assign the values 
        rec->id = ids[i];
        rec->flags = flags[i];
        rec->code = codes[i];
    }

    for (uint8_t i = 0; i < MAX_RECORDS; i++) { 
        // getting the Record back from the block
        Record *rec = (Record *)((char *)block + (i * record_capacity));
        printf("Record ID  : %llu\n", rec->id);
        printf("Record FLAG: %d\n", rec->flags);
        printf("Record CODE: %u\n", rec->code);
        printf("------------------------\n");
    }

    free(block);

    return 0;
}
