#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief enum used to test the generic arrays implementation
 */
typedef enum SupportedTypedValue
{
    INT64, DATA 
} SupportedTypedValue;

/**
 * @brief struct used to the generic arrays implementation 
 */
typedef struct tshirt {
    double value;
    uint16_t id;
    char size;
    char color;
} tshirt;

/**
 * @brief Holds both the typed element distance and raw byte distance
 *        between two pointers in a contiguous array.
 */
struct pointer_distance_report {
    ptrdiff_t element_distance; /**< Number of elements between start and end (end - start) */
    ptrdiff_t byte_distance;    /**< Number of raw bytes between start and end */
    size_t    element_stride;   /**< sizeof(*start) in bytes */
};

/**
 * @brief Computes the element count distance and raw byte distance between
 *        two typed `uint64_t` pointers within the same contiguous array.
 *
 * @param start Pointer to the starting element. Must not be NULL.
 * @param end   Pointer to the ending element. Must not be NULL.
 * @param T     enum SupportedTypedValue
 * @return A `struct pointer_distance_report` containing the element distance,
 *         raw byte distance, and element stride size.
 */
struct pointer_distance_report measure_pointer_distance(const void *start, const void *end, enum SupportedTypedValue T);

/**
 * @brief Writes a single element of `elem_size` bytes from `src_elem` into
 *        a generic contiguous array at position `index` using raw byte strides.
 *
 * @param base      Base address of the contiguous target array. Must not be NULL.
 * @param count     Total number of elements allocated in the array.
 * @param elem_size Size in bytes of a single element (stride). Must be > 0.
 * @param index     Zero-based element index to write to. Must satisfy index < count.
 * @param src_elem  Pointer to the source element data to copy from. Must not be NULL.
 * @return 0 on success, or -1 if any pointer is NULL, elem_size is 0, or index >= count.
 */
int8_t generic_array_write(void *base, size_t count, size_t elem_size, size_t index, const void *src_elem);

/**
 * @brief Reads a single element of `elem_size` bytes from a generic contiguous
 *        array at position `index` into `out_elem` using raw byte strides.
 *
 * @param base      Base address of the contiguous source array. Must not be NULL.
 * @param count     Total number of elements allocated in the array.
 * @param elem_size Size in bytes of a single element (stride). Must be > 0.
 * @param index     Zero-based element index to read from. Must satisfy index < count.
 * @param out_elem  Pointer to the destination buffer where the element will be copied.
 *                  Must not be NULL.
 * @return 0 on success, or -1 if any pointer is NULL, elem_size is 0, or index >= count.
 */
int8_t generic_array_read(const void *base, size_t count, size_t elem_size, size_t index, void *out_elem);

/**
 * @brief Helper method used to test the generic array implementation (WRITE) 
 * @param buffer void pointer
 * @param src_elements void pointer
 * @param quantity size_t
 * @param T SupportedTypedValue enum 
 * @return void
 */
void simulate_packets_shipping(void *buffer, void *src_elements, size_t quantity, enum SupportedTypedValue T);

/**
 * @brief Helper method used to test the generic array implementation (READ)
 * @param buffer void pointer
 * @param src_elements void pointer
 * @param quantity size_t
 * @param T SupportedTypedValue enum 
 * @return void
 */
void simulate_packets_reading(void *buffer, void *src_elements, size_t quantity, enum SupportedTypedValue T);

/**
 * @brief Helper method used to test the distance between two elements
 * @param start void pointer
 * @param end void pointer
 * @param T enum SupportedTypedValue
 * @return void
 */
void calculate_elements_distance(void *start, void *end, enum SupportedTypedValue T);

/**
 * @brief Helper method used to prints out the stored element
 * @param base void pointer
 * @param count size_t
 * @param T enum SupportedTypedValue
 * @return void
 */
void print_elements(void *base, size_t count, enum SupportedTypedValue T); 

/**
 * @brief Frees allocated objects
 * @param buffer void pointer
 * @param elements void pointer
 * @return void
 */
void free_resources(void *buffer, void *elements);

// MAIN METHOD -------------------------------------------------------------------------------------------
int main(void) {
    
    size_t requested_chunks = 3; 
    
    /// TESTING array<T:uint64_t> ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
    printf("\nTESTING array<T:uint64_t>\n");

    void *buffer = calloc(1, (requested_chunks * sizeof(uint64_t)));
    if (buffer == NULL) exit(EXIT_FAILURE);
    
    // Simulating 3 incoming packets to stored in the generic buffer of type uint64_t
    void *src_elements = calloc(1, sizeof(uint64_t) * requested_chunks);
    if (src_elements == NULL) exit(EXIT_FAILURE);

    // Sending 3 packets of type uint64_t
    simulate_packets_shipping(buffer, src_elements, requested_chunks, INT64);
    // Reading 3 packets of type uint64_t
    simulate_packets_reading(buffer, src_elements, requested_chunks, INT64);
   
    // prints all elements of type uint64_t
    print_elements(buffer, requested_chunks, INT64);

    // prove index >= count guardrail catches an out-of-bounds write at runtime and returns -1
    uint64_t dummy_val = 999;
    assert(generic_array_write(buffer, requested_chunks, sizeof(uint64_t), 3, &dummy_val) == -1);
    assert(generic_array_read(buffer, requested_chunks, sizeof(uint64_t), 3, &dummy_val) == -1);

    // computing the distance between element at index 0 and 2 for type uint64_t 
    calculate_elements_distance(((char *)buffer), ((char *)buffer + (2 * sizeof(uint64_t))), INT64);
    
    // free all allocated resources in the heap
    free_resources(buffer, src_elements);
    printf("-----------------------------------------\n");
    
    /// TESTING array<T:tshirt> >> :::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: 
    printf("\nTESTING array<T:tshirt>\n");
    
    void *data_buffer = calloc(1, (requested_chunks * sizeof(struct tshirt)));
    if (data_buffer == NULL) exit(EXIT_FAILURE);
    
    src_elements = calloc(1, sizeof(struct tshirt) * requested_chunks);
    if (src_elements == NULL) exit(EXIT_FAILURE);

    // Sending 3 packets of type uint64_t
    simulate_packets_shipping(data_buffer, src_elements, requested_chunks, DATA);
    // Reading 3 packets of type struct tshirt
    simulate_packets_reading(data_buffer, src_elements, requested_chunks, DATA);

    // prints all elements of type struct tshirt
    print_elements(data_buffer, requested_chunks, DATA);
    
    // computing the distance between element at index 0 and 2 for type uint64_t 
    calculate_elements_distance(((char *)data_buffer), ((char *)data_buffer + (2 * sizeof(struct tshirt))), DATA);

    // free all allocated resources in the heap
    free_resources(data_buffer, src_elements);
    printf("-----------------------------------------\n");
    
    return 0;
}

// IMPLEMENTATION ----------------------------------------------------------------------------------------

struct pointer_distance_report measure_pointer_distance(const void *start, const void *end, enum SupportedTypedValue T) {
    
    if (start == NULL || end == NULL) 
    {
        fprintf(stderr, "Pointer start or end cannot be NULL\n");
        exit(EXIT_FAILURE);
    }
    

    switch (T)
    {
        case INT64:
            {
                // compute element distance 
                ptrdiff_t element_distance = ((uint64_t *)end - (uint64_t *)start);
                // casting before calculating the raw distance 
                const uint8_t *i_start = (const uint8_t *)start;
                const uint8_t *i_end = (const uint8_t *)end;
                // compute raw/bytes distance
                ptrdiff_t raw_distance = (i_end - i_start);
                
                assert((uint64_t)raw_distance == (sizeof(uint64_t) * (uint64_t)element_distance));
    
                struct pointer_distance_report report;
                report.element_distance = element_distance;
                report.byte_distance = raw_distance;
                report.element_stride = sizeof(*(const uint64_t *)start);

                return report;
            }
        case DATA:
            {
                const struct tshirt *e_start = (const struct tshirt *)start;
                const struct tshirt *e_end = (const struct tshirt *)end;
                
                // compute element distance 
                ptrdiff_t element_distance = (e_end - e_start);
                // casting before calculating the raw distance 
                const uint8_t *i_start = (const uint8_t *)e_start;
                const uint8_t *i_end = (const uint8_t *)e_end;
                // compute raw/bytes distance
                ptrdiff_t raw_distance = (i_end - i_start);
               
                assert(raw_distance == (int64_t)(sizeof(struct tshirt) * element_distance));

                struct pointer_distance_report report;
                report.element_distance = element_distance;
                report.byte_distance = raw_distance;
                report.element_stride = sizeof(*e_start);

                return report;
            }
        default:
            fprintf(stderr, "Error: Unsupported type\n");
            exit(EXIT_FAILURE);
    }
}

int8_t generic_array_write(void *base, size_t count, size_t elem_size, size_t index, const void *src_elem) {
  
    if (base == NULL || elem_size == 0 || index >= count || src_elem == NULL) return -1;
    
    // Finding the starting byte 
    uint8_t *dst_bytes = (uint8_t *)base + (index * elem_size);
    // Point to the first byte of the caller's destination box (src_elem)
    const uint8_t *src_bytes = (const uint8_t *)src_elem;
    // Copy elem_size bytes one by one from the slot into the caller's box
    for (size_t b = 0; b < elem_size; b++) {
        *(dst_bytes + b) = *(src_bytes + b);
    }
    
    return 0;
}

int8_t generic_array_read(const void *base, size_t count, size_t elem_size, size_t index, void *out_elem) {
   
    if (base == NULL || elem_size == 0 || index >= count || out_elem == NULL) return -1;
   
    // Finding the starting byte 
    const uint8_t *src_bytes = (const uint8_t *)base + (index * elem_size);
    // Point to the first byte of the caller's destination box (out_elem)
    uint8_t *dst_bytes = (uint8_t *)out_elem;
    // Copy elem_size bytes one by one from the slot into the caller's box
    for (size_t b = 0; b < elem_size; b++) {
        *(dst_bytes + b) = *(src_bytes + b);
    }

    return 0;
}


void simulate_packets_shipping(void *buffer, void *src_elements, size_t quantity, enum SupportedTypedValue T) {
    
    if(buffer == NULL || src_elements == NULL)
    {
        fprintf(stderr, "Error: Buffer not allocated\n");
        exit(EXIT_FAILURE);
    }

    switch (T) 
    {
        case INT64:
            {
                // setting the size for INT64 (uint64_t)
                size_t elem_size = sizeof(uint64_t);
                // preparing packets to be send 
                uint64_t values[3] = {11, 111, 1111};
                
                for (uint8_t i = 0; i < quantity; i++) {
                    uint64_t *src_elem = (uint64_t *)((char *)src_elements + (i * elem_size));
                    *src_elem = *(values + i);
                }
                
                for (uint8_t i = 0; i < quantity; i++) {
                    uint64_t *element = (uint64_t *)((char *)src_elements + (i * elem_size));
                    printf("Sending packet[%d] with value: %llu\n", i, *element);
                    // writing packet
                    int8_t write_state = generic_array_write(buffer, quantity, elem_size, i, (void *)element);
        
                    if (write_state == -1) 
                    {
                        fprintf(stderr, "Error writing the element with value: %llu\n", *element);
                        exit(EXIT_FAILURE);
                    }
                }
                
                break;
            }
        case DATA:
            {
                size_t elem_size = sizeof(struct tshirt);
                // create the 3 t-shirts 
                struct tshirt values[3];

                struct tshirt t1;
                t1.value = 14.99;
                t1.id = 1;
                t1.size = 'S';
                t1.color = 'R';

                struct tshirt t2;
                t2.value = 7.99;
                t2.id = 2;
                t2.size = 'L';
                t2.color = 'W';

                struct tshirt t3;
                t3.value = 22.99;
                t3.id = 3;
                t3.size = 'M';
                t3.color = 'B';
                
                *(values + 0) = t1;
                *(values + 1) = t2;
                *(values + 2) = t3;
                
                for (uint8_t i = 0; i < quantity; i++) {
                    struct tshirt *src_elem = (struct tshirt *)((char *)src_elements + (i * elem_size));
                    *src_elem = *(values + i);
                }

                for (uint8_t i = 0; i < quantity; i++) {
                    struct tshirt *element = (struct tshirt *)((char *)src_elements + (i * elem_size));
                    printf("Sending packet[%d] with size: %c\n", i, element->size);
                    // writing packets
                    int8_t write_state = generic_array_write(buffer, quantity, elem_size, i, (void *)element);
                    
                    if (write_state == -1) 
                    {
                        fprintf(stderr, "Error writing the element with size: %c\n", element->size);
                        exit(EXIT_FAILURE);
                    }
                }

                break;
            }
        default:
            fprintf(stderr, "Error: Unsupported type\n");
            exit(EXIT_FAILURE);
    }

}

void simulate_packets_reading(void *buffer, void *src_elements, size_t quantity, enum SupportedTypedValue T) {
       
    if(buffer == NULL || src_elements == NULL)
    {
        fprintf(stderr, "Error: Buffer not allocated\n");
        exit(EXIT_FAILURE);
    }

    switch (T)
    {
        case INT64:
            { 
                // clean to all zeros to prove data will be read from memory block
                memset(src_elements, 0, quantity * sizeof(uint64_t));
                
                size_t elem_size = sizeof(uint64_t);

                for (uint8_t i = 0; i < quantity; i++) {
                    uint64_t *element = (uint64_t *)((char *)src_elements + (i * elem_size));
                    // read packet 
                    int8_t read_state = generic_array_read(buffer, quantity, elem_size, i, (void *)element);

                    if (read_state == -1)
                    {
                        fprintf(stderr, "Error reading the element with value: %llu\n", *element);
                        exit(EXIT_FAILURE);
                    }
                }
                
                break;
            }
        case DATA:
            {
                // clean to all zeros to prove data will be read from memory block
                memset(src_elements, 0, quantity * sizeof(struct tshirt));
                
                size_t elem_size = sizeof(struct tshirt);

                for (uint8_t i = 0; i < quantity; i++) {
                    struct tshirt *element = (struct tshirt *)((char *)src_elements + (i * elem_size));
                    // read packet
                    int8_t read_state = generic_array_read(buffer, quantity, elem_size, i, (void *)element);
                    
                    if (read_state == -1)
                    {
                        fprintf(stderr, "Error reading the element with size: %c\n", element->size);
                        exit(EXIT_FAILURE);
                    }
                }

                break;
            }
        default:
            fprintf(stderr, "Error: Unsupported type\n");
            exit(EXIT_FAILURE);

    }
}

void calculate_elements_distance(void *start, void *end, enum SupportedTypedValue T) {
    
    if (start == NULL || end == NULL) 
    {
        fprintf(stderr, "Error: start or end pointer is NULL\n");
        exit(EXIT_FAILURE);
    }

    const uint64_t *e_start = (uint64_t *)start;
    const uint64_t *e_end = (uint64_t *)end;
    struct pointer_distance_report rep = measure_pointer_distance(e_start, e_end, T);
    // element distance 
    printf("Elements distance: %td\n", rep.element_distance);
    printf("Raw distance     : %td\n", rep.byte_distance);
    printf("Stride           : %td\n", rep.element_stride);
}

void print_elements(void *base, size_t count, enum SupportedTypedValue T) {
    
    if (base == NULL) 
    {
        fprintf(stderr, "Error: Buffer is  NULL\n");
        exit(EXIT_FAILURE);
    }

    switch (T)
    {
        case INT64:
            {
                size_t elem_size = sizeof(uint64_t);
                
                for (uint8_t i = 0; i < count; i++) {
                    uint64_t *e = (uint64_t *)((char *)base + (i * elem_size));
                    printf("Read value stored at element[%d]: %llu\n", i, *e);    
                }

                break;
            }
        case DATA:
            {
                size_t elem_size = sizeof(struct tshirt);

                for(uint8_t i = 0; i < count; i++) {
                    struct tshirt *e = (struct tshirt *)((char *)base + (i * elem_size));
                    printf("Reading data[%d]:\n", i);
                    printf("\tTshirt's id   : %d\n", e->id);
                    printf("\tTshirt's value: %.2f\n", e->value);
                    printf("\tTshirt's size : %c\n", e->size);
                    printf("\tTshirt's color: %c\n", e->color);
                    printf(".....................................\n");
                }

                break;
            }
        default:
            fprintf(stderr, "Error: Unsupported type\n");
            exit(EXIT_FAILURE);

    }
    
}

void free_resources(void *buffer, void *elements) {
    if (elements != NULL) free(elements);
    if (buffer != NULL) free(buffer);
}
