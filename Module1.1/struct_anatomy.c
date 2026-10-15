#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

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
    size_t raw_size = sizeof(RawEventMeta);
    size_t com_size = sizeof(CompactEventMeta);

    printf("Size of RawEventMeta: %zu\n", raw_size);
    printf("Align of RawEventMeta: %zu\n", _Alignof(RawEventMeta));
    printf("-------------------------------------\n");
    printf("Size of CompactEventMeta: %zu\n", com_size);
    printf("Align of CompactEventMeta: %zu\n", _Alignof(CompactEventMeta));
    printf("-------------------------------------\n");

    show_offsets(RAW);
    printf("-------------------------------------\n");
    show_offsets(COM);
    printf("-------------------------------------\n");

    RawEventMeta dummy_raw;
    size_t useful_bytes_raw = sizeof(dummy_raw.version) +
        sizeof(dummy_raw.timestamp_ns) +
        sizeof(dummy_raw.source_id) +
        sizeof(dummy_raw.payload_len) +
        sizeof(dummy_raw.flags);
    
    CompactEventMeta dummy_com;
    size_t useful_bytes_com = sizeof(dummy_com.version) +
        sizeof(dummy_com.timestamp_ns) +
        sizeof(dummy_com.source_id) +
        sizeof(dummy_com.payload_len) +
        sizeof(dummy_com.flags);
    
    
    printf("Useful bytes RawEventMeta    : %zu\n", useful_bytes_raw);
    printf("Useful bytes CompactEventMeta: %zu\n", useful_bytes_com);
    printf("-------------------------------------\n");

    int64_t wasted_padding_raw = raw_size - useful_bytes_raw;
    int64_t wasted_padding_com = com_size - useful_bytes_com;

    printf("Wasted RawEventMeta padding    : %llu\n", wasted_padding_raw);
    printf("Wasted CompactEventMeta padding: %llu\n", wasted_padding_com);
    printf("-------------------------------------\n");

    size_t total_bytes_saved = (sizeof(RawEventMeta) - sizeof(CompactEventMeta)) * packtes_num;

    printf("Total bytes saved: %zu\n", total_bytes_saved);

}


void show_offsets(StructType type) {
    
    switch (type) {
        case RAW:
            printf("Offset of RawEventMeta->version     : %zu\n", offsetof(RawEventMeta, version));
            printf("Offset of RawEventMeta->timestamp_ns: %zu\n", offsetof(RawEventMeta, timestamp_ns));
            printf("Offset of RawEventMeta->source_id   : %zu\n", offsetof(RawEventMeta, source_id));
            printf("Offset of RawEventMeta->payload_len : %zu\n", offsetof(RawEventMeta, payload_len));
            printf("Offset of RawEventMeta->flags       : %lu\n", offsetof(RawEventMeta, flags));
            break;
        case COM:
            printf("Offset of CompactEventMeta->timestamp_ns: %zu\n", offsetof(CompactEventMeta, timestamp_ns));
            printf("Offset of CompactEventMeta->payload_len : %zu\n", offsetof(CompactEventMeta, payload_len));
            printf("Offset of CompactEventMeta->source_id   : %zu\n", offsetof(CompactEventMeta, source_id));
            printf("Offset of CompactEventMeta->version     : %zu\n", offsetof(CompactEventMeta, version));
            printf("Offset of CompactEventMeta->flags       : %zu\n", offsetof(CompactEventMeta, flags));
            break;
        default:
            fprintf(stderr, "Invalid Struct type\n");
            break;
    }
}
