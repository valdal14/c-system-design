#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    uint8_t *buffer;
    size_t capacity;
    size_t current_offset;
} Arena;

/**
 * @brief Allocates the Arena (a block of pre-allocated heap memory)
 * @param a Arena double pointer
 * @param size_t size 
 * @return void
 */
void arena_init(Arena **a, size_t size);

/**
 * @brief Retuns a void pointer to the next available block
 * of pre-allocated memory
 * @param a Arena pointer
 * @param requested_size size_t
 * @return void pointer
 */
void *arena_alloc(Arena *a, size_t requested_size);

/**
 * @brief Alloc a new block from the Arena and store a value 
 * of block_size on the pointer location. Return 1 if the value
 * has been stored 0 if not.
 * 
 * @param a Arena pointer
 * @param value uint8_t
 * @param block_size size_t
 * @return uint8_t
 */
uint8_t alloc_and_store(Arena *a, uint8_t value, size_t block_size);

/**
 * @brief Prints out the value stored in a block allocated in the Arena. 
 *
 * @param a Arena pointer
 * @param block_size size_t
 * @param offset size_t
 * @return void
 */
void retrieve(Arena *a, size_t block_size, size_t offset);

/**
 * @brief Simulates incoming packages for testing the 
 * Arena behaviour
 *
 * @param a Arena pointer
 * @param values uint8_t
 * @param values_size uint8_t
 * @param block_size uint8_t
 * @return void
 */
void simulate_incoming_data(Arena *a, uint8_t *values, uint8_t values_size, uint8_t block_size);

/**
 * @brief Cleans up allocated memory
 * @param a Arena pointer
 * @return void
 */
void clean(Arena *a);

/// MAIN METHOD /////////////////////////////////////////////////////////////////////////

int main(void) {
    Arena *arena = NULL;
    // allocate the Arena
    arena_init(&arena, 254);

    // variable used for the testing the Arena
    uint8_t testing_values[3] = {67, 128, 234};
    uint8_t testing_val_size = sizeof(testing_values) / sizeof(testing_values[0]);
    uint8_t block_size = 127;
    size_t strating_offset = 0;

    // Arena logic test 
    simulate_incoming_data(arena, testing_values, testing_val_size, block_size);
    retrieve(arena, block_size, strating_offset);
    clean(arena);
    
    return 0;
}


/// IMPLEMENTATION //////////////////////////////////////////////////////////////////////

void arena_init(Arena **a, size_t size) {
    Arena *arena = (Arena *)calloc(1, sizeof(Arena));
    if (arena == NULL) exit(EXIT_FAILURE);

    arena->buffer = (uint8_t *)calloc(1, sizeof(uint8_t) * size);
    if (arena->buffer == NULL) exit(EXIT_FAILURE);

    arena->capacity = size;
    arena->current_offset = 0;
    *a = arena;
}

void *arena_alloc(Arena *a, size_t requested_size) {
    if (requested_size > a->capacity || (a->current_offset + requested_size) > a->capacity) {
        fprintf(stderr, "The requested size overun the current Arena's capacity\n");
        return NULL;
    } else {
        // allocate a new block by adding the current_offset to the buffer 
        void *buffer = (void *)((char *)a->buffer + a->current_offset);
        a->current_offset += sizeof(char) * requested_size;
        return buffer;
    }
}


uint8_t  alloc_and_store(Arena *a, uint8_t value, size_t block_size) {
    // Allocate a new block
    void *new_block = arena_alloc(a, block_size);

    if (new_block != NULL) {
        // cast assign a value to that memory block
        uint8_t *v1 = (uint8_t *)((char *)new_block);
        // dereference and assign value to the address 
        *v1 = value;
        return 1;
    } else {
        return 0;
    }
}


void retrieve(Arena *a, size_t block_size, size_t offset) {
    // use the buffer plus the offset to get to the correct value 
    uint8_t *val = (uint8_t *)((char *)a->buffer + offset);
    if (offset >= a->current_offset || val == NULL) return;
    printf("Value %d retrieved from block address %p\n", *val, val);
    offset += block_size;
    retrieve(a, block_size, offset); 
}


void simulate_incoming_data(Arena *a, uint8_t *values, uint8_t values_size, uint8_t block_size) {
    
    for (uint8_t i = 0; i < values_size; i++) {
        
        uint8_t n = alloc_and_store(a, values[i], block_size);
        
        if (n == 1) {
            printf("Value %d stored in the new allocated block\n", values[i]);
        } else {
            fprintf(stderr, "Value %d could not be stored\n", values[i]);
        }
    }
}

void clean(Arena *a) {
    if (a->buffer != NULL) free(a->buffer);
    free(a);
}
