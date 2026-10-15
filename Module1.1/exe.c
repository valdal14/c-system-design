#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define BLOCK_SIZE 32 
#define CHUNK_SIZE 8

/**
 * @brief Calculates and returns the exact memory address of an element at a given index 
 *
 * @param base void pointer
 * @param count size_t
 * @param elem_size size_t
 * @param index size_t
 * @return void pointer 
 */
void *array_element_at(void *base, size_t count, size_t elem_size, size_t index);

/**
 * @brief Allocates a single block and chunks the memory based on the chunks
 * size and alignment
 *
 * @param block_size size_t
 * @return void pointer
 */
void *init_memory_block(size_t block_size);

/**
 * @brief Allocates a new element into the pre-allocated memeory block 
 *
 * @param block void pointer
 * @param value uint32_t 
 * @param requested_size size_t
 * @param alignment size_t
 * @param current_offset size_t
 * @return size_t
 */
size_t alloc_element(void *block, uint32_t value, size_t requested_size, size_t alignment, size_t current_offset);

int main(void) 
{
    size_t current_offset = 0;
    size_t alignments[4] = {4,1,8,4};
    uint32_t values[4] = {14, 23, 11, 25};
    
    void *block = init_memory_block(BLOCK_SIZE);
   
    // alloc new chunks with data
    for (uint8_t i = 0; i < (BLOCK_SIZE / CHUNK_SIZE); i++) 
        current_offset = alloc_element(block, values[i], CHUNK_SIZE, alignments[i], current_offset);
    
    // reset the current_offset
    current_offset = 0;

    // retrieve values 
    for (uint8_t i = 0; i < (BLOCK_SIZE / CHUNK_SIZE); i++) {
        uint8_t *p = (uint8_t *)((char *)block + current_offset);
        printf("stored value: %d\n", *p);
        current_offset += CHUNK_SIZE;
    }

    // get value at index
    for (uint8_t i = 0; i < (BLOCK_SIZE / CHUNK_SIZE) + 1; i++) {
        void *element = (void *)array_element_at(block, 4, CHUNK_SIZE, i);
        
        if (element != NULL) {
            uint32_t *val = (uint32_t *)((char *)element);
            printf("element value at given index is: %u\n", *val);
        }
    }
    
    free(block);

    return 0;
}


void *array_element_at(void *base, size_t count, size_t elem_size, size_t index) {

    if(base == NULL || elem_size == 0 || index >= count)
    {
        fprintf(stderr, "Invalid arguments\n");
        return NULL;
    }
    
    uint8_t *base_bytes = (uint8_t *)base;
    return (void *)(base_bytes + (index * elem_size));
}


void *init_memory_block(size_t block_size) {
    
    if (block_size == 0)
    {
        fprintf(stderr, "Invalid block_size or chunks_size\n");
        exit(EXIT_FAILURE);
    }

    void *memory_block = calloc(1, sizeof(char) * block_size);

    if (memory_block == NULL) 
    {
        fprintf(stderr, "Could not allocate memory block\n");
        exit(EXIT_FAILURE);
    }

    return memory_block;
}


size_t alloc_element(void *block, uint32_t value, size_t requested_size, size_t alignment, size_t current_offset) {
    
    size_t modulo = current_offset % alignment;
    size_t padding = (modulo == 0) ? 0 : (alignment - modulo);

    if ((requested_size > BLOCK_SIZE) || (current_offset + padding + requested_size > BLOCK_SIZE))
    {
        fprintf(stderr, "The requested size overun the current capacity\n");
        exit(EXIT_FAILURE);
    }

    current_offset += padding;

    uint32_t *num = (uint32_t *)((char *)block + current_offset);
    *num = value;

    current_offset += requested_size;

    return current_offset;
}
