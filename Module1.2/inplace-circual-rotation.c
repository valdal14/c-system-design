#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum SupportedType
{
    INT64, INT32 
};

struct Buffer
{
    void *pool;
    size_t index;
    size_t elem_size;
};

/**
 * @brief Circularly rotates a generic contiguous array to the left by `k` positions
 *        completely in-place.
 *
 * @param base      Base address of the contiguous array. Must not be NULL.
 * @param count     Total number of elements in the array.
 * @param elem_size Size in bytes of a single element (stride). Must be > 0.
 * @param k         The number of positions to rotate left. (May be greater than `count`).
 * @return 0 on success, or -1 if base is NULL or elem_size is 0.
 */
int8_t generic_array_rotate_left(void *base, size_t count, size_t elem_size, size_t k);

/**
 * @brief Helper function that returns the size based on the 
 * given SupportedType and the number of chunks
 *
 * @param T enum SupportedType
 * @param chunks size_t
 * @return size_t 
 */
size_t get_size(enum SupportedType T, size_t chunks); 

/**
 * @brief Init a buffer and chunks it given the number of chunks
 * and the size of its type
 *
 * @param chunks size_t
 * @param elem_size size_t
 * @param callback void pointer function
 * @return struct Buffer pointer 
 */
struct Buffer *init(size_t chunks, enum SupportedType T, size_t (*callback)(enum SupportedType T, size_t chunks));

/**
 * @brief Allocates a single value of type SupportedType T into 
 * the pre-allocated buffer 
 *
 * @param buffer struct Buffer pointer
 * @param value void pointer
 * @param T SupportedType enum
 * @return void
 */
void alloc(struct Buffer *buffer, void *value, enum SupportedType T);

/**
 * @brief Prints out the elements stored in the pool  
 *
 * @param buffer struct Buffer pointer
 * @param T enum SupportedType
 * @return void
 */
void print_element(struct Buffer *buffer, enum SupportedType T);

/**
 * @brief Reverses a block of memory by its count and elem_size
 *
 * @param block void pointer
 * @param count size_t
 * @param elem_size size_t
 * @return int8_t: 0 if success -1 otherwise
 */
int8_t generic_array_reverse(void *block, size_t count, size_t elem_size);


/**
 * @brief Helper function used to swap two void pointer 
 *
 * @param a void pointer
 * @param b void pointer
 * @param elem_size size_t
 * @return int8_t: 0 if success -1 otherwise
 */
int8_t generic_elements_swap(void *a, void *b, size_t elem_size);

/**
 * @brief Helper funtion used to assert the rotate left on generic arrays
 *
 * @param base void pointer
 * @param expected_output uint32_t array
 * @param count size_t
 * @return void
 */
void assert_rotate_left(void *base, uint32_t expected_output[], size_t count);

int main(void) 
{
    size_t number_of_elements = 5;
    uint32_t values[] = {10, 20, 30, 40, 50};

    // init the buffer 
    struct Buffer *buffer = init(number_of_elements, INT32, get_size);
    // allocate new value to the buffer
    for (uint8_t i = 0; i < number_of_elements; i++) {
        uint32_t *p = (values + i);
        alloc(buffer, p, INT32);
    } 

    // prints current state
    print_element(buffer, INT32);
    // rotate the pool elements
    int8_t res = generic_array_rotate_left(buffer->pool, buffer->index, buffer->elem_size, 2);

    if (res == 0)
    {
        print_element(buffer, INT32);
    }

    // asserting the rotation changes
    uint32_t expected_output[] = {30, 40, 50, 10, 20};
    assert_rotate_left(buffer->pool, expected_output, number_of_elements);

    free(buffer->pool);
    free(buffer);
    return 0;
}


size_t get_size(enum SupportedType T, size_t chunks) {
    
    switch (T)
    {
        case INT32:
            {
                return sizeof(uint32_t) * chunks;
            }
        case INT64:
            {
                return sizeof(uint64_t) * chunks;
            }
        default:
            {
                fprintf(stderr, "Error: Unsupported type\n");
                exit(EXIT_FAILURE);
            }
    }
}


struct Buffer *init(size_t chunks, enum SupportedType T, size_t (*callback)(enum SupportedType T, size_t chunks)) {

    size_t buffer_size = callback(T, chunks);

    if (buffer_size <= 0)
    {
        fprintf(stderr, "Invalid size calculation, the application will be closed\n");
        exit(EXIT_FAILURE);
    }

    struct Buffer *buffer = (struct Buffer *)calloc(1, sizeof(struct Buffer));
    buffer->pool = calloc(1, sizeof(char) * buffer_size);

    if (buffer == NULL || buffer->pool == NULL) 
    {
        fprintf(stderr, "Error: Could not allocated buffer\n");
        exit(EXIT_FAILURE);
    }

    buffer->index = 0;
    buffer->elem_size = (buffer_size / chunks);
    return buffer;
}


void alloc(struct Buffer *buffer, void *value, enum SupportedType T) {

    switch (T)
    {
        case INT32:
            { 
                if (value == NULL)
                {
                    fprintf(stderr, "Error: Casting to uint32_t failed\n");
                    exit(EXIT_FAILURE);
                }
                
                // cast the value pointer to get its uint32_t value 
                uint32_t *val = (uint32_t *)value;
                uint32_t element = *val;

                if (buffer->elem_size != sizeof(element))
                {
                    fprintf(stderr, "Invalid type size for type T\n");
                    exit(EXIT_FAILURE);
                }
                
                // write the value to the buffer 
                uint32_t *p_val = (uint32_t *)((char *)buffer->pool + (buffer->index * buffer->elem_size));
                *p_val = element;
                
                printf("Stored value %u in the pool\n", *p_val);
                
                // increase the index 
                buffer->index += 1;

                break;
            }
        case INT64:
            {
                // cast the value pointer to get its uint64_t value 
                uint64_t *val = (uint64_t *)value;
                
                if (val == NULL)
                {
                    fprintf(stderr, "Error: Casting to uint64_t failed\n");
                    exit(EXIT_FAILURE);
                }

                if (buffer->elem_size != sizeof(*val))
                {
                    fprintf(stderr, "Invalid type size for type T\n");
                    exit(EXIT_FAILURE);
                }

                // write the value to the buffer 
                uint64_t *p_val = (uint64_t *)((char *)buffer->pool + (buffer->index * buffer->elem_size));
                *p_val = *val;

                printf("Stored value %llu in the pool\n", *p_val);
                
                // increase the index 
                buffer->index += 1;

                break;
            }
        default:
            {
                fprintf(stderr, "Error: Unsupported type\n");
                exit(EXIT_FAILURE);
            } 
    }
}

void print_element(struct Buffer *buffer, enum SupportedType T) {
    
    if (buffer->index == 0) {
        printf("Pool is empty, there is nothing to print\n");    
    }

    switch (T)
    {
        case INT32:
            {
                for (uint8_t i = 0; i < buffer->index; i++) {
                    uint32_t *element = (uint32_t *)((char *)buffer->pool + (i * buffer->elem_size));
                    printf("index[%d] = %u\n", i, *element);
                }
                
                break;
            }
        case INT64:
            {
                for (uint8_t i = 0; i < buffer->index; i++) {
                    uint64_t *element = (uint64_t *)((char *)buffer->pool + (i * buffer->elem_size));
                    printf("index[%d] = %llu\n", i, *element);
                }
                
                break;
            }
        default:
            {
                fprintf(stderr, "Error: Unsupported type\n");
                exit(EXIT_FAILURE);
            } 
    }

    printf("--------------------------\n");

}

int8_t generic_array_rotate_left(void *base, size_t count, size_t elem_size, size_t k) {
    
    if (base == NULL || count <= 0 || elem_size <= 0)
    {
        fprintf(stderr, "Error: array rotation cannot be performed, please check your input values\n");
        return -1;
    }

    // normalize k for value greater than the count of the elements
    k = k % count;
    if (k == 0) return 0;
    
    // get the first block 
    void *first_k_block = ((char *)base);
    int8_t first_slice = generic_array_reverse(first_k_block, k, elem_size);
    
    if (first_slice != 0)
    {
        fprintf(stderr, "Error: An attempt to reverse a block of memory failed\n");
        return -1;
    }

    // grab the second portion of the pool from 0 to k * elem_size 
    void *second_k_block = ((char *)base + (k * elem_size));
    // count - k element size 
    int8_t second_slice = generic_array_reverse(second_k_block, (count - k), elem_size);

    if (second_slice != 0)
    {
        fprintf(stderr, "Error: An attempt to reverse a block of memory failed\n");
        return -1;
    }

    // finally reverse the entire block 
    int8_t res = generic_array_reverse(base, count, elem_size);

    if (res != 0)
    {
        fprintf(stderr, "Error: An attempt to reverse a block of memory failed\n");
        return -1;
    }

    return 0;
}

int8_t generic_array_reverse(void *block, size_t count, size_t elem_size) {
    
    if (block == NULL) return -1;
    // if count is less or equal to 1 the reverse is not needed.
    if (count <= 1) return 0;

    // grab the first element
    uint8_t *left = (uint8_t *)((char *)block);
    // grab the last element
    uint8_t *right = (uint8_t *)((char *)block + ((count - 1) * elem_size));
    
    while (left < right)
    {
        int8_t res = generic_elements_swap(left, right, elem_size);
        if (res != 0) return -1;
        left += elem_size;
        right -= elem_size;
    }

    return 0;
}

int8_t generic_elements_swap(void *a, void *b, size_t elem_size) {
    
    if (a == NULL || b == NULL)
    {
        fprintf(stderr, "Error: Impossible swap NULL pointers\n");
        return -1;
    }

    uint8_t *a_bytes = (uint8_t *)a;
    uint8_t *b_bytes = (uint8_t *)b;
    
    for (size_t i = 0; i < elem_size; i++) {
        uint8_t temp = *(a_bytes + i);
        *(a_bytes + i) = *(b_bytes + i);
        *(b_bytes + i) = temp;
    }

    return 0;
}

void assert_rotate_left(void *base, uint32_t expected_output[], size_t count) {
    
    if (base == NULL) 
    {
        fprintf(stderr, "Test failed with NULL base\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < count; i++) {
        uint32_t *current = (uint32_t *)((char *)base + (i * sizeof(uint32_t)));
        uint32_t *expected = (expected_output + i);
        assert(*current == *expected);
    }
}
