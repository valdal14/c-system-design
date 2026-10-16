#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCK_SIZE 240
#define CHUNKS 10 

typedef struct {
    uint64_t record_id; 
    double  metric_value;
    uint32_t batch_epoch;  
    uint16_t partition_key; 
    uint8_t status_code; 
    uint8_t is_dirty;  
} optimal_record;

/**
 * @brief Dynamically allocates a contiguous block of optimal_record instances on the heap,
 *        populates them sequentially, and verifies stride addresses using raw pointer arithmetic
 *        without relying on array indexing brackets `[]`.
 *
 * @param count Number of records to allocate.
 * @return void* Pointer to the heap allocation (or 0/NULL on failure).
 */
void *allocate_and_inspect_pipeline_buffer(size_t count);

int main(void) 
{  
    assert(sizeof(optimal_record) == 24);
    
    printf("--- ALLOCATION ---\n");
    void *buffer = allocate_and_inspect_pipeline_buffer(CHUNKS);
    
    printf("--- RETRIEVAL ---\n");
    for (uint8_t i = 0; i < CHUNKS; i++) {
        optimal_record *record = (optimal_record *)((char *)buffer + (i * (BLOCK_SIZE / CHUNKS)));
        // assert each retrieved record_id
        assert(record->record_id == i + 1);
        printf("Record ID: %llu\n", record->record_id);
    }

    free(buffer);
    
    return 0;
}


void *allocate_and_inspect_pipeline_buffer(size_t count) {
    void *mem_block = calloc(1, sizeof(optimal_record));
    
    if (mem_block == NULL) {
        fprintf(stderr, "Could not allocate space for the memory block\n");
        exit(EXIT_FAILURE);
    }

    if (BLOCK_SIZE % count == 0) 
    {
        for (uint8_t i = 0; i < count; i++) {
            optimal_record *record = (optimal_record *)((char *)mem_block + (i * (BLOCK_SIZE / count)));    
            // filling just the id for testing purposes
            record->record_id = i + 1;
            printf("chunk[%d] allocated in the main memory block at address: %p\n", i, (void *)record);
        }
        
        return mem_block;
    
    } else {
        fprintf(stderr, "Invalid count\n");
        exit(EXIT_FAILURE);
    }
}
