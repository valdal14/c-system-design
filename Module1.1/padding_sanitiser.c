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
    
    return 0;
}


size_t count_nonzero_padding_bytes(const RawEventMeta *meta) {
    size_t version = offsetof(RawEventMeta, version) / sizeof(meta->version);
    size_t timestamp = offsetof(RawEventMeta, timestamp_ns) / sizeof(meta->timestamp_ns);
    size_t source = offsetof(RawEventMeta, source_id) / sizeof(meta->source_id);
    size_t payload = offsetof(RawEventMeta, payload_len) / sizeof(meta->payload_len);
    size_t flags = offsetof(RawEventMeta, flags) / sizeof(meta->flags);
    return version + timestamp + source + payload + flags;
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
