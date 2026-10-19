#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ELEMENTS 50 

/**
 * @brief Swaps the raw bytes of two elements of `elem_size` bytes in-place
 * using a single 1-byte stack variable.
 *
 * @param elem_a Pointer to the first element in RAM. Must not be NULL.
 * @param elem_b Pointer to the second element in RAM. Must not be NULL.
 * @param elem_size Size in bytes of a single element. Must be > 0.
 * @return 0 on success, or -1 if elem_a is NULL, elem_b is NULL, or elem_size is 0.
 */
int8_t generic_byte_swap(void *elem_a, void *elem_b, size_t elem_size);

/**
 * @brief Reverses the order of `count` elements in a generic contiguous array
 * in-place using two-pointer byte-stride arithmetic and `generic_byte_swap`.
 *
 * @param base Base address of the contiguous array to reverse. Must not be NULL.
 * @param count Total number of elements in the array. (If count == 1, no-op returning 0).
 * @param elem_size Size in bytes of a single element (stride). Must be > 0.
 * @return 0 on success, or -1 if base is NULL, count is 0, or elem_size is 0.
 */
int8_t generic_array_reverse(void *base, size_t count, size_t elem_size);

/**
 * @brief Inits the memory pool
 *
 * @param pool uint64_t double pointer
 * @param count size_t
 * @param const values uint64_t pointer 
 * @return void pointer
 */
void *init_pool(size_t count, const uint64_t *values);

/**
 * @brief Verofies that the pool is not NULL and the count is 
 * not greater than the MAX_ELEMENTS
 *
 * @param base void pointer
 * @param count size_t
 * @return 0 on success, or -1 if base is NULL, count is 0, or elem_size is 0.
 */
int8_t verify(void *base, size_t count, size_t elem_size);


int main(void) 
{
    uint64_t values[] = {500, 400, 300, 200, 100, 50, 10, 0};
    size_t count = sizeof(values) / sizeof(values[0]);

    uint64_t *buffer = init_pool(count, values);
    
    int8_t res = generic_array_reverse(buffer, count, sizeof(uint64_t));
    
    if (res == -1)
    {
        fprintf(stderr, "reversal process failed\n");
        exit(EXIT_FAILURE);
    }

    for (uint8_t i = 0; i < count; i++) {
        uint64_t *v = (uint64_t *)((char *)buffer + (i * sizeof(uint64_t)));
        printf("Value stored: %llu\n", *v);
    }

    free(buffer);

    return 0;
}

void *init_pool(size_t count, const uint64_t *values) { 
    
    uint64_t *base = (uint64_t *)calloc(1, sizeof(uint64_t) * count + 1);
    if (verify(base, count, sizeof(uint64_t)) != 0) exit(EXIT_FAILURE);
    
    for (uint8_t i = 0; i < count; i++) {
        uint64_t *v = (uint64_t *)((char *)base + (i * sizeof(uint64_t)));
        *v = *(values + i);
        printf("Stored initial value %llu at index %d\n", *v, i);
    }

    return base;
}


int8_t generic_array_reverse(void *base, size_t count, size_t elem_size) {
    
    if (verify(base, count, elem_size) != 0) return -1;
    if (count == 1) return 0;

    uint8_t iteration = (uint8_t)count / 2;
    size_t counter = count - 1;
    size_t size = 0;

    for (uint8_t i = 0; i < iteration; i++) {
        uint64_t *start = (uint64_t *)((char *)base + size);
        uint64_t *end = (uint64_t *)((char *)base + ((counter) * elem_size));
        
        int8_t res = generic_byte_swap(start, end, elem_size);
        if (res != 0) return -1;
        
        size += elem_size;
        counter--;
    }
    
    return 0;
}

int8_t generic_byte_swap(void *elem_a, void *elem_b, size_t elem_size) {
    
    if (elem_a == NULL || elem_b == NULL || elem_size <= 0) return -1;

    uint64_t temp = *(uint64_t *)elem_a;
    *(uint64_t *)elem_a = *(uint64_t *)elem_b;
    *(uint64_t *)elem_b = temp;

    return 0;
}

int8_t verify(void *base, size_t count, size_t elem_size) {
    
    if (count > MAX_ELEMENTS || count <= 0 ||  elem_size <= 0) 
    {
        fprintf(stderr, "Error: invalid count or elem_size\n");
        return -1;
    }

    if (base == NULL) {
        fprintf(stderr, "Pool is NULL, the application will be terminated\n");
        return -1;
    }

    return 0; 
}
