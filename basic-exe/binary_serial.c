#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * @brief Reads from the single block of allocate memory
 * and cast to pointers of different types to access to 
 * the information stored at specific location addresses
 *
 * @param frame void pointer
 * @return void
 */
void read_frame(void *frame) { 
    uint16_t *id = (uint16_t *)((char *)frame);
    uint32_t *crc = (uint32_t *)((char *)frame + sizeof(uint16_t));
    char *message = (char *)((char *)frame + sizeof(uint32_t));

    printf("Message ID  : %d\n", *id);
    printf("Source MSN  : %d\n", *crc);
    printf("Message BODY: %s\n", message);
}

int main(void) {
    const char *source_msg = "OXFORD_26";
    uint16_t msn_len = strlen(source_msg) + 1;
    uint16_t msn_id = 14;
    uint32_t source_crc = 99887766;
    
    size_t current_offset = 0;

    size_t size = sizeof(uint16_t) + sizeof(uint32_t) + msn_len;

    void *memory = (void *)calloc(1, sizeof(char) * size);
    if (memory == NULL) exit(EXIT_FAILURE);
    // allocate from the memory block and assign a value to 
    // the pointer destination 
    uint16_t *id = (uint16_t *)((char *)memory + current_offset);
    *id = msn_id;
    current_offset = sizeof(uint16_t);

    uint32_t *crc = (uint32_t *)((char *)memory + current_offset);
    *crc = source_crc;
    current_offset += sizeof(uint32_t);

    // write at memory[0] + offset uint32_t where the crc is stored
    char *message = (char *)((char *)memory + current_offset);
    memcpy(message, source_msg, msn_len);

    read_frame(memory);
    free(memory);

    return 0;
}
