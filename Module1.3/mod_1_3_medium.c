#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN_CHUNKS 2
#define MAX_MEMORY_ALLOCATION 1024

/**
 * @brief Specify the supported data type that can be stored
 * inside the memory Pool. 
 */
typedef enum BlockType
{
    INT32, INT64 
} BlockType;

/**
 * @brief Defines the Data Structure of the memory Pool  
 */
typedef struct
{
    void *buffer; 
    size_t max_allocation; 
    size_t capacity; 
    size_t current_size; 
    size_t element_size; 
    BlockType type; 
    // 
    // Law II — Struct-Level Alignment
    // 48 bytes in total with max element size 8 bytes 
    // 48 % 8 = 0
    //
    // Law III — Tail Padding (The Array Stride Rule)
    // NOTE: Last member BlockType of size 4 bytes 
    //
    // sizeof(Pool) - (offsetof(Pool -> BlockType type) + sizeof(type))
    // 48 - (44 + 4) = 0 
} Pool;

/**
 * @brief Allocates the memory pool for the current data processing pipeline
 *
 * @param pool Pool double pointer
 * @param T BlockType (currently supported uint64_t and uint32_t)
 * @return int8_t: 0 for success or 1 for failure
 */
int8_t alloc(Pool **pool, BlockType T); 

/**
 * @brief Adds a new value to the memory Pool
 *
 * @param pool Pool double pointer
 * @param value void pointer
 * @return int8_t: 0 for success or 1 for failure
 */
int8_t add_data(Pool *pool, void *value);

/**
 * @brief Resises the memory buffer by doubling its current capacity when adding 
 * a new value. If the current size will exceed the capacity it will perform 
 * a realloc call 
 *
 * @param pool Pool double pointer
 * @return int8_t: 0 for success or 1 for failure
 */
int8_t resize_pool(Pool *pool);

/**
 * @brief Clean up all allocated objects from the heap
 *
 * @param pool Pool pointer
 * @return void
 */
void clean(Pool *pool);

/// MAIN METHOD ---------------------------------------------------------------------------

int main(void)
{
    // allocate the pool 
    Pool *pool = NULL;
    int8_t pool_alloc_result = alloc(&pool, INT32);
    
    if (pool_alloc_result != 0)
    {
        fprintf(stderr, "Could not allocate space for the pool\n");
        exit(EXIT_FAILURE);
    }

    // add values to the pool 
    uint32_t values[] = {11, 22, 33, 44, 55, 66, 77, 88, 99};

    for (size_t i = 0; i < (sizeof(values) / sizeof(uint32_t)); i++) {
        
        int8_t add_result = add_data(pool, (void *)(values + i));

        if (add_result != 0)
        {
            fprintf(stderr, "Could not add a new value to the pool\n");
            exit(EXIT_FAILURE);
        }

        printf("%u was added to the pool at index: %zu\n", *(values + i), pool->current_size - 1);
        printf("pool capacity    : %zu\n", pool->capacity);
        printf("pool current size: %zu\n", pool->current_size);
        uint32_t *p = (uint32_t *)((char *)pool->buffer + (pool->element_size * (pool->current_size - 1)));
        printf("value stored at index[%zu]: %u\n", i, *p); 
        printf("--------------------------------------------------\n");
    }

    // assert the memory footprint
    assert(pool->capacity == 16);
    assert(pool->current_size == 9);
    assert((MAX_MEMORY_ALLOCATION - (pool->current_size * pool->element_size)) == 988);

    // free the allocated memory
    clean(pool);

    return 0;
}

/// IMPLEMENTATION ------------------------------------------------------------------------

int8_t alloc(Pool **pool, BlockType T) {

    if (pool == NULL) return -1; 

    Pool *new_pool = (Pool *)calloc(1, sizeof(Pool));
    if (new_pool == NULL) return -1;
    // stores the size of the given BlockType 
    size_t type_size = 0;
    
    switch (T)
    {
        case INT32:
            {
                type_size = sizeof(uint32_t);
                break;
            }
        case INT64:
            {
                type_size = sizeof(uint64_t);
                break;
            }
        default:
            {
                return -1;
            }
    }

    if (type_size == 0) return -1;

    new_pool->buffer = calloc(1, sizeof(char) * (type_size * MIN_CHUNKS));
    if (new_pool->buffer == NULL) return -1;
    
    new_pool->capacity = MIN_CHUNKS;
    new_pool->current_size = 0;
    new_pool->max_allocation = MAX_MEMORY_ALLOCATION;
    new_pool->element_size = type_size;
    new_pool->type = T;

    *pool = new_pool;
    return 0;
}

int8_t add_data(Pool *pool, void *value) {
    
    if(pool == NULL || value == NULL) return -1;

    switch (pool->type)
    {
        case INT32:
            {
                uint32_t *val = (uint32_t *)value;
                if (sizeof(*val) != pool->element_size) return -1;

                if (((pool->current_size + 1) * pool->element_size) > pool->max_allocation)
                {
                    fprintf(stderr, "Pool reached max allocation capacity. Reallocation not allowed\n");
                    return -1;
                }

                if ((pool->current_size + 1) > pool->capacity) {
                    // start the reallocation process 
                    int8_t reallocation_result = resize_pool(pool);
                    
                    if (reallocation_result != 0)
                    {
                        fprintf(stderr, "Reallocation of the Pool failed\n");
                        return -1;
                    }
                }
                
                uint32_t *p = (uint32_t *)((char *)pool->buffer + (pool->element_size * pool->current_size));
                *p = *val;

                pool->current_size += 1;

                break;
            }
        case INT64:
            {
                uint64_t *val = (uint64_t *)value;
                if (sizeof(*val) != pool->element_size) return -1;
                printf("uint64_t %llu\n", *val);
                break;
            }
        default:
            {
                return -1;
            }
    }


    return 0;
}

int8_t resize_pool(Pool *pool) {
                    
    printf("Reallocation in progress. New capacity will be: %zu\n", pool->capacity * MIN_CHUNKS);
    void *alloc_res = realloc(pool->buffer, pool->element_size * (pool->capacity * MIN_CHUNKS));

    if (alloc_res == NULL) {
        return -1;
    } else { 
        // set buffer equals to the new alloc_res pointer 
        pool->buffer = alloc_res;
        // set the new capacity
        size_t new_capacity = (pool->capacity * MIN_CHUNKS);
        pool->capacity = new_capacity;
        printf("New Capacity: %zu\n", pool->capacity);
        return 0;
    }
}

void clean(Pool *pool) {
    if (pool == NULL) return;
    if (pool->buffer != NULL) free(pool->buffer);
    if (pool != NULL) free(pool);
}

