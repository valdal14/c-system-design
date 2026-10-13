#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef enum {
    BIT32, BIT64
} OffsetType;

/**
 * @brief Increase the offset based on the given OffsetType
 * @param size_t current_offset
 * @param value OffsetType
 * @return size_t
 */
size_t set_offset(size_t current_offset, OffsetType value) {
    switch (value) {
        case BIT32:
            return current_offset + sizeof(uint32_t);
        case BIT64:
            return current_offset + sizeof(uint64_t);
        default:
            fprintf(stderr, "Unsupported type\n");
            exit(EXIT_FAILURE);
    }
}

/**
 * @brief Reads the value from the void frame by casting and 
 * getting the value back from the main memory block
 * @param frame void pointer
 * @return void
 */
void read_sensor_frame(void *frame) {
    
    size_t offset = 0;

    uint32_t *id = (uint32_t *)((char *)frame + offset);
    offset = set_offset(offset, BIT32);

    uint64_t *s_ts = (uint64_t *)((char *)frame + offset);
    offset = set_offset(offset, BIT64);

    char *s_name = (char *)((char *)frame + offset);

    printf("Source MSN ID: %d\n", *id);
    printf("Source MSN TS: %llu\n", *s_ts);
    printf("Source NAME  : %s\n", s_name);
}

int main(void) {
    uint32_t source_id = 8844;
    const char *source_name = "TEMP_VALVE";
    size_t n_len = strlen(source_name) + 1;
    uint64_t source_ts = 1700000000;

    size_t total_size = sizeof(source_id) + n_len + sizeof(source_ts);

    void *frame = (void *)calloc(1, sizeof(char) * total_size);
    if (frame == NULL) exit(EXIT_FAILURE);

    // set initial offset value 
    size_t current_offset = 0;

    // cast and add value to the pointer location 
    uint32_t *id = (uint32_t *)((char *)frame + current_offset);
    *id = source_id;
    current_offset = set_offset(current_offset, BIT32);
    
    uint64_t *s_ts = (uint64_t *)((char *)frame + current_offset);
    *s_ts = source_ts;
    current_offset = set_offset(current_offset, BIT64);

    char *s_name = (char *)((char *)frame + current_offset);
    memcpy(s_name, source_name, n_len);

    // read the stored values 
    read_sensor_frame(frame);

    // clean up allocation
    free(frame);

    return 0;
}
