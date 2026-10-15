#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

/**
 * @breif Allocates an align an object in the heap
 *
 * @param count size_t
 * @param elem_size size_t
 * @param alignment size_t
 */
void *aligned_calloc_array(size_t count, size_t elem_size, size_t alignment);

/**
 * @breif Frees and align a given object
 *
 * @param aligned_ptr void pointer
 */
void aligned_free(void *aligned_ptr);

int main(void) {
    size_t chunk_size = 2;
    size_t block_size = 8;
    size_t alignment = 8;

    // allocation, init  and alignment
    void *buffer = aligned_calloc_array(block_size, chunk_size, alignment);

    // retrieve values and print them 
    for(uint8_t i = 0; i < (block_size / chunk_size); i++) {
        uint8_t *p = (uint8_t *)((char *)buffer + (i + chunk_size));
        printf("stored value: %d\n", *p);
    }

    // free allocation 
    aligned_free(buffer);

    return 0;
}


void *aligned_calloc_array(size_t count, size_t elem_size, size_t alignment) {
    
    uint8_t base_val = alignment;

    if (count == 0 || elem_size == 0) return NULL;

    void *buffer = (void *)calloc(1, (sizeof(char) * count) + alignment);
    
    if (buffer == NULL)  {
        perror("Could not allocated memory block");
        exit(EXIT_FAILURE);
    }

    for(uint8_t i = 0; i < (count / elem_size); i++) {
        uint8_t *p = (uint8_t *)((char *)buffer + (i + elem_size));
        *p = i * base_val;
    }

    if (((uintptr_t)buffer % alignment) == 0) {
        return buffer;
    } else {
        perror("Invalid memory block alignment");
        return NULL;
    }
}


void aligned_free(void *aligned_ptr) {
    if (aligned_ptr != NULL) free(aligned_ptr);
}

