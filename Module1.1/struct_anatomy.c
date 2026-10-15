#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>

typedef struct {
    uint8_t  version;
    uint64_t timestamp_ns;
    uint16_t source_id;
    uint32_t payload_len;
    uint8_t  flags;
} RawEventMeta;

typedef struct {
    uint64_t timestamp_ns;
    uint32_t payload_len;
    uint16_t source_id;
    uint8_t  version;
    uint8_t  flags;
    // 8 + 4 + 2 + 1 + 1 = 16 bytes 0 paddings
} CompactEventMeta;

typedef enum {
    RAW, COM
} StructType;

/**
 * @brief Audit the structs layouts
 * return void
 */
void audit_struct_layout(void);

/**
 * @brief Prints out the offset of each struct's type 
 * based on the given StructType
 *
 * @param type StructType
 * @return void
 */
void show_offsets(StructType type);

int main(void) {
    audit_struct_layout();
    return 0;
}

void audit_struct_layout(void) { 
    uint64_t packtes_num = 50000000;

    printf("Size of RawEventMeta: %zu\n", sizeof(RawEventMeta));
    printf("Align of RawEventMeta: %lu\n", _Alignof(RawEventMeta));
    printf("-------------------------------------\n");
    printf("Size of CompactEventMeta: %zu\n", sizeof(CompactEventMeta));
    printf("Align of CompactEventMeta: %lu\n", _Alignof(CompactEventMeta));
    printf("-------------------------------------\n");

    show_offsets(RAW);
    printf("-------------------------------------\n");
    show_offsets(COM);
    printf("-------------------------------------\n");

    uint64_t sum1 = (offsetof(RawEventMeta, version) +
        offsetof(RawEventMeta, timestamp_ns) +
        offsetof(RawEventMeta, source_id) +
        offsetof(RawEventMeta, payload_len) +
        offsetof(RawEventMeta, flags)) * packtes_num;
    

    uint64_t sum2 = (offsetof(CompactEventMeta, version) +
        offsetof(CompactEventMeta, timestamp_ns) +
        offsetof(CompactEventMeta, source_id) +
        offsetof(CompactEventMeta, payload_len) +
        offsetof(CompactEventMeta, flags)) * packtes_num;
    
    printf("Bytes sum for RawEventMeta    : %llu\n", sum1);
    printf("Bytes sum for CompactEventMeta: %llu\n", sum2);

    printf("Total bytes saved: %llu\n", (sum1 - sum2));

}


void show_offsets(StructType type) {
    
    switch (type) {
        case RAW:
            printf("Offset of RawEventMeta->version %lu\n", offsetof(RawEventMeta, version));
            printf("Offset of RawEventMeta->timestamp_ns %lu\n", offsetof(RawEventMeta, timestamp_ns));
            printf("Offset of RawEventMeta->source_id %lu\n", offsetof(RawEventMeta, source_id));
            printf("Offset of RawEventMeta->payload_len %lu\n", offsetof(RawEventMeta, payload_len));
            printf("Offset of RawEventMeta->flags %lu\n", offsetof(RawEventMeta, flags));
            break;
        case COM:
            printf("Offset of CompactEventMeta->timestamp_ns %lu\n", offsetof(CompactEventMeta, timestamp_ns));
            printf("Offset of CompactEventMeta->payload_len %lu\n", offsetof(CompactEventMeta, payload_len));
            printf("Offset of CompactEventMeta->source_id %lu\n", offsetof(CompactEventMeta, source_id));
            printf("Offset of CompactEventMeta->version %lu\n", offsetof(CompactEventMeta, version));
            printf("Offset of CompactEventMeta->flags %lu\n", offsetof(CompactEventMeta, flags));
            break;
        default:
            perror("Invalid Struct type\n");
            break;
    }
}
