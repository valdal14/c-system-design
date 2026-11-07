#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

typedef struct
{
    size_t len;
    size_t alloc;
    uint8_t *buf;
} SDS;

/**
 * @brief Allocates a new SDS 
 *
 * @param capacity size_t
 * @return SDS struct
 */
SDS *sds_new(size_t capacity);

/**
 * @brief Appends new data to the buffer 
 *
 * @param sds SDS struct pointer
 * @param const data uint8_t pointer
 * @param data_len size_t
 * @return void
 */
void sds_append(SDS *sds, const uint8_t *data, size_t data_len);

/**
 * @brief Cleans up the allocated memory
 *
 * @param sds SDS struct pointer
 * @return void
 */
void sds_free(SDS *sds);

int main(void) {
    // 1. Initialize with capacity for 4 bytes
    SDS *my_string = sds_new(4);
    assert(my_string->alloc == 4);
    assert(my_string->len == 0);

    // 2. The Poison Pill: 4 bytes containing a Null Terminator (0x00)
    uint8_t payload1[4] = { 0x41, 0x00, 0x42, 0x43 }; 
    
    sds_append(my_string, payload1, 4);
    assert(my_string->len == 4);
   
    // 3. The Reallocation Trigger: 3 more bytes
    uint8_t payload2[3] = { 0xAA, 0xBB, 0xCC };
    
    sds_append(my_string, payload2, 3);
    assert(my_string->len == 7);
    assert(my_string->alloc >= 7); // Capacity should have expanded!

    // 4. Prove binary safety by printing the raw hex
    printf("Final Length: %zu\n", my_string->len);
    printf("Final Capacity: %zu\n", my_string->alloc);
    printf("Raw Memory: ");
    for (size_t i = 0; i < my_string->len; i++) {
        printf("%02X ", my_string->buf[i]);
    }
    printf("\n");
    
    // Expected Output: 41 00 42 43 AA BB CC
    
    // 5. Clean up
    sds_free(my_string);
    printf("SDS Architecture: PASSED\n");

    return 0;
}

SDS *sds_new(size_t capacity) {
    
    if (capacity <= 0) return NULL;
    SDS *sds = (SDS *)calloc(1, sizeof(SDS)); 
    if (sds == NULL) return NULL;

    sds->buf = calloc(capacity, sizeof(uint8_t));
    if (sds->buf == NULL) return NULL;
    
    sds->alloc = capacity;
    sds->len = 0;
    return sds;
}


void sds_append(SDS *sds, const uint8_t *data, size_t data_len) {
    
    if (sds == NULL || data == NULL || data_len <= 0) return;

    if ((sds->len + data_len) > sds->alloc) 
    {
        size_t new_size = sds->alloc * 2;
        
        if (new_size < (sds->len + data_len)) {
            new_size = sds->len + data_len; 
        }

        void *ptr = realloc(sds->buf, (sizeof(char) * new_size));
        if (ptr == NULL) return;
        sds->buf = ptr;
        sds->alloc = new_size;
    }

    for (size_t i = 0; i < data_len; i++) {
        // get the value from the const uint8_t data 
        uint8_t *val = (uint8_t *)((char *)data + (i * sizeof(uint8_t)));
        // stores the values 
        uint8_t *p = (uint8_t *)((char *)sds->buf + (sds->len * sizeof(uint8_t)));
        *p = *val;
        sds->len += 1;
    }
}

void sds_free(SDS *sds) {
    
    if (sds == NULL) return;
    
    free(sds->buf);
    free(sds);
}
