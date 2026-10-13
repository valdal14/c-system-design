#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief Increase the offset based on the given OffsetType
 * @param size_t current_offset
 * @return size_t
 */
void read_sensor_frame(void *frame) {
    size_t offset = 0;

    uint8_t *msn_size = (uint8_t *)((char *)frame + offset);
    offset += sizeof(uint8_t);

    uint32_t *sensor_id = (uint32_t *)((char *)frame + offset);
    offset += sizeof(uint32_t);

    char *s_name = (char *)((char *)frame + offset);
    // get the next offset size from the len of msn_size
    // update the offset with the len of the string 
    offset += *msn_size;
    
    uint64_t *ts = (uint64_t *)((char *)frame + offset);
    
    printf("ID  : %d\n", *sensor_id);
    printf("TS  : %llu\n", *ts);
    printf("MSN : %s\n", s_name);
}

int main(void) {    
    uint32_t source_id = 8844;
    const char *source_name = "TEMP_VALVE";
    size_t n_len = strlen(source_name) + 1;
    uint64_t source_ts = 1700000000;

    // The size of the id, the string, the timestamp and finally the len of the string itself as num
    // to peform O(1) lookup 
    size_t total_size = sizeof(source_id) + n_len + sizeof(source_ts) + sizeof(uint8_t);

    void *frame = (void *)calloc(1, sizeof(char) * total_size);
    if (frame == NULL) exit(EXIT_FAILURE);

    // set initial offset value
    size_t current_offset = 0;

    uint8_t *msn_size = (uint8_t *)((char *)frame + current_offset);
    *msn_size = n_len;
    current_offset += sizeof(uint8_t);
    
    // cast and add value to the pointer location
    uint32_t *id = (uint32_t *)((char *)frame + current_offset);
    *id = source_id;
    current_offset += sizeof(uint32_t);

    char *s_name = (char *)((char *)frame + current_offset);
    memcpy(s_name, source_name, n_len);
    current_offset += sizeof(char) * n_len;
    
    uint64_t *s_ts = (uint64_t *)((char *)frame + current_offset);
    *s_ts = source_ts;

    // read the stored values
    read_sensor_frame(frame);

    // clean up allocation
    free(frame);

    return 0;
}
