#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

enum SupportedType 
{
    INT32, INT64
};

/**
 * @brief Allocates a contiguous block of heap memory and updates the caller's pointer 
 * to own the new memory block.
 *
 * @param target A pointer to the caller's uninitialized pointer (Out-Parameter). Must not be NULL.
 * @param count The number of elements to allocate. Must be > 0.
 * @param elem_size The size of each element in bytes. Must be > 0.
 * @return 0 on success, or -1 if target is NULL, count is 0, elem_size is 0, or allocation fails.
 */
int8_t generic_allocate(void **target, size_t count, size_t elem_size);

/**
 * @brief Add values to the previously allocated memory block
 *
 * @param target void double pointer
 * @param value void pointer 
 * @param elem_size size_t
 * @param size_t index 
 * @param T enum SupportedType
 * @return int8_t 0 on success or -1 on failure 
 */
int8_t add_data(void *target, void *value, size_t elem_size, size_t index, enum SupportedType T);

/**
 * @brief Prints out the chunks's value stored in the memory block
 *
 * @param target void pointer
 * @param elem_size size_t
 * @param count size_t
 * @param T enum SupportedType
 * @return void
 */
void print_stored_elements(void *target, size_t elem_size, size_t count, enum SupportedType T);

/**
 * @brief Cleans up the allocated memory
 *
 * @param target void pointer
 * @return void
 */
void clean(void *target);

/// MAIN ---------------------------------------------------------------------------------------

int main(void) 
{
    uint64_t data[] = {100, 200, 300, 400, 500};

    void *block = NULL;
    int8_t alloc_result = generic_allocate(&block, 5, sizeof(uint64_t));
    
    if (alloc_result != 0)
    {
        fprintf(stderr, "Could not allocate space for the new block\n");
    }
   
    // adding data to the block's chunks 
    for (size_t i = 0; i < 5; i++) {
        int8_t add_result = add_data(block, (void *)(data + i), sizeof(uint64_t), i, INT64);
        
        if (add_result != 0)
        {
            fprintf(stderr, "There was an error while adding the value %llu\n", (uint64_t)(data + i));
            exit(EXIT_FAILURE);
        }
    }

    // prints elements
    print_stored_elements(block, sizeof(uint64_t), 5, INT64);
    // cleanups 
    clean(block);

    return 0;
}

/// IMPLEMENTATION -----------------------------------------------------------------------------

int8_t generic_allocate(void **target, size_t count, size_t elem_size) {
    
    if (target == NULL || count <= 0 || elem_size <= 0) return -1;

    void *memory_block = calloc(1, sizeof(char) * (count * elem_size));
    if (memory_block == NULL) return -1;

    for (size_t i = 0; i < count; i++) {
        void *block = (void *)((char *)memory_block + (i * elem_size));
        printf("chunk of size %zu bytes was allocated at address %p\n", elem_size, block);
    }

    *target = memory_block;
    return 0;
}

int8_t add_data(void *target, void *value, size_t elem_size, size_t index, enum SupportedType T) {

    if (target == NULL || value == NULL || elem_size <= 0) return -1;

    switch (T)
    {
        case INT32:
            {
                // get the pointer from the block based on the current index and its size 
                uint32_t *p = (uint32_t *)((char *)target + (index * elem_size));
                // cast the value to a uint32_t
                uint32_t *v = (uint32_t *)((char *)value);
                // check the sizes match 
                if (sizeof(*v) != elem_size) return -1;
                // assign the value to the chunk 
                *p = *v;
                return 0;
            }
        case INT64:
            {
                // get the pointer from the block based on the current index and its size 
                uint64_t *p = (uint64_t *)((char *)target + (index * elem_size));
                // cast the value to a uint64_t
                uint64_t *v = (uint64_t *)((char *)value);
                // check the sizes match 
                if (sizeof(*v) != elem_size) return -1;
                // assign the value to the chunk 
                *p = *v;
                return 0;
            }
        default:
            {
                fprintf(stderr, "Unsupported type\n");
                return -1;
            }
    }
}


void print_stored_elements(void *target, size_t elem_size, size_t count, enum SupportedType T) {

    if (target == NULL || elem_size <= 0 || count <= 0) return;

    switch (T)
    {
        case INT32:
            {
                printf("\n.::. uint32_t element output .::.\n");
                
                for (size_t i = 0; i < count; i++) {
                    uint32_t *p = (uint32_t *)((char *)target + (i * elem_size));
                    printf("target[%zu] = %u stored at address %p\n", i, *p, (void *)p);
                    assert(sizeof(*p) == elem_size);
                }

                break;
            }
        case INT64:
            {
                printf("\n.::. uint64_t element output .::.\n");
                
                for (size_t i = 0; i < count; i++) {
                    uint64_t *p = (uint64_t *)((char *)target + (i * elem_size));
                    printf("target[%zu] = %llu stored at address %p\n", i, *p, (void *)p);
                    assert(sizeof(*p) == elem_size);
                }

                break;
            }
        default:
            {
                fprintf(stderr, "Unsupported type\n");
                return;
            }
    }
}


void clean(void *target) {
    if (target == NULL) return;
    free(target);
}
