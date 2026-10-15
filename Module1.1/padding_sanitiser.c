#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

typedef struct {
    uint8_t  version;
    uint64_t timestamp_ns;
    uint16_t source_id;
    uint32_t payload_len;
    uint8_t  flags;
} RawEventMeta;

/**
 * @brief Counts non-zero padding bytes
 *
 * @param meta const RawEventMeta
 * @return size_t 
 */
size_t count_nonzero_padding_bytes(const RawEventMeta *meta);

/**
 * @brief Allocates a new instance of the RawEventMeta on the heap 
 *
 * @param meta RawEventMeta double pointer
 * @return void
 */
void init_meta(RawEventMeta **meta);

int main(void) {
    RawEventMeta *heap_meta = NULL;
    init_meta(&heap_meta);

    RawEventMeta stack_meta;
    // fill the stack_meta struct simulating stack garbage
    memset((void *)&stack_meta, 0xFF, sizeof(RawEventMeta));
    stack_meta.version = 1;
    stack_meta.timestamp_ns = 20261015;
    stack_meta.source_id = 111;
    stack_meta.payload_len = 14;
    stack_meta.flags = 4;

    size_t stack_nonzero_bytes = count_nonzero_padding_bytes(&stack_meta);
    size_t heap_nonzero_bytes = count_nonzero_padding_bytes(heap_meta);
    printf("Stack Struct: %zu\n", stack_nonzero_bytes);
    printf("Heap Struct : %zu\n", heap_nonzero_bytes);

    if (memcmp(&stack_meta, heap_meta, sizeof(RawEventMeta)) == 0) {
        printf("Heap and Stack RawEventMeta are identical\n");
    } else {
        printf("Heap and Stack RawEventMeta are NOT identical\n"); 
    }
    
    free(heap_meta);
    return 0;
}


size_t count_nonzero_padding_bytes(const RawEventMeta *meta) {
    
    if(meta == NULL) {
        fprintf(stderr, "Unexpected  RawEventMeta pointer was passed\n");
        exit(EXIT_FAILURE);
    }

    const uint8_t *raw_bytes = (const uint8_t *)meta;
    size_t nonzero_count = 0;

    size_t gap_start = offsetof(RawEventMeta, version) + sizeof(meta->version);
    size_t gap_end   = offsetof(RawEventMeta, flags);

    for (size_t i = gap_start; i < gap_end; i++) {
        if (raw_bytes[i] != 0) {
            nonzero_count++;
        }    
    }

    return nonzero_count;
}

void init_meta(RawEventMeta **meta) {
    RawEventMeta *heap_meta = (RawEventMeta *)calloc(1, sizeof(RawEventMeta));
    
    if (heap_meta == NULL) {
        perror("Could not allocate RawEventMeta struct on the heap");
        exit(EXIT_FAILURE);
    }

    heap_meta->version = 1;
    heap_meta->timestamp_ns = 20261015;
    heap_meta->source_id = 111;
    heap_meta->payload_len = 14;
    heap_meta->flags = 4;
    
    *meta = heap_meta;
}
