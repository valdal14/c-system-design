#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Wire-frame header containing both internal padding and tail padding.
 */
struct frame_header {
    /**
     * offset 0 and alignment 1
     * 0 mod 1 = 0 
     */
    uint8_t  magic;  
    /**
     * starting offset 1 alignment 8
     * (1 + 8) = 9 
     * 1 mode 8 = 1 therefore paddings are needed 
     * they will be inserted at the beginning to 
     * get to a multiple of 8 (uint64_t)
     *
     * 1 (original offset) + 7 (padding) + 8 (alignment) = 16
     * 16 mod 8 = 0
     */
    uint64_t sequence_id;  
    /**
     * current offset 16 and alignment 1 
     * 16 mod 1 = 0
     */
    uint8_t  flags;
    /**
     * current offset 17 and alignment 2
     * 17 mod 2 = 1 therefore paddings are needed
     * 17 + 1 (padding) mod 2 = 0
     * new offset 18 and alignment 2 
     */
    uint16_t checksum;

    /**
     * Law II — Struct-Level Alignment ----------------
     * 
     * The size of frame_header should be 20 
     * max(1, 8, 2) = 8 bytes given by the type uint64_t
     * 20 mod 8 = 4 and this does not adhere to Law II 
     *
     * NOTE: Therefore the compiler will add 4 extra paddings 
     * to adhere to Law III 
     * ------------------------------------------------
     *
     *  Law III — Tail Padding (The Array Stride Rule)
     *
     *  Last Member: uint16_t checksum of size 2 bytes 
     *  sizeof(struct frame_header) - (offsetof(checksum) + sizeof(checksum))
     *  20 + 4 (tail padding) - (18 + 2) = 4
     *
     *  Therefore the actual sizeof(struct frame_header) = 24 bytes
     */
};

/**
 * @brief Summary metrics returned by the byte-by-byte memory inspector.
 */
struct byte_inspection_summary {
    size_t total_bytes_inspected; /**< Total bytes walked (sizeof(struct frame_header)) */
    size_t payload_bytes_count;   /**< Number of bytes belonging to active struct members */
    size_t internal_pad_count;    /**< Number of bytes classified as INTERNAL_PADDING */
    size_t tail_pad_count;        /**< Number of bytes classified as TAIL_PADDING */
};

/**
 * @brief Walks a `struct frame_header` in RAM one byte at a time using raw
 *        `const uint8_t *` pointer arithmetic, printing each byte's offset,
 *        virtual address, 2-digit hex value, and region classification.
 *
 * @param hdr Pointer to the initialized `struct frame_header` instance in RAM.
 *            Must not be NULL.
 * @return A `struct byte_inspection_summary` containing the exact counts of
 *         payload, internal padding, and tail padding bytes encountered.
 */
struct byte_inspection_summary inspect_struct_bytes(const struct frame_header *hdr);

/**
 * @brief Init a void pointer using the method declaration params
 *
 * @param sequence_id uint64_t
 * @param checksum uint16_t
 * @param magic uint8_t
 * @param flags uint8_t
 * @return struct frame_header pointer
 */
struct frame_header *init(uint64_t sequence_id, uint16_t checksum, uint8_t magic, uint8_t flags); 

int main(void)
{
    struct frame_header *header = init(0x1122334455667788ULL, 0xCCDD, 0xAA, 0xBB); 
    struct byte_inspection_summary inspect = inspect_struct_bytes(header);
    printf("PAYLOAD BYTES    : %zu\n", inspect.payload_bytes_count);
    printf("INTERNAL PADDINGS: %zu\n", inspect.internal_pad_count);
    printf("TAIL PADDINGS    : %zu\n", inspect.tail_pad_count);
    printf("----------------------\n");
    printf("TOTAL BYTES      : %zu\n", inspect.total_bytes_inspected);

    free(header);
    return 0;
}


struct frame_header *init(uint64_t sequence_id, uint16_t checksum, uint8_t magic, uint8_t flags) {

   struct frame_header *header = calloc(1, sizeof(struct frame_header));

   if(header == NULL)
   {
       fprintf(stderr, "Error allocating the struct frame_header\n");
       exit(EXIT_FAILURE);
   }

   memset(header, 0xAA, sizeof(struct frame_header));

   header->sequence_id = sequence_id;
   header->checksum = checksum;
   header->magic = magic;
   header->flags = flags;

   return header;
}

struct byte_inspection_summary inspect_struct_bytes(const struct frame_header *hdr) {
    // assert the pointer is not NULL
    assert(hdr != NULL);
    
    // statically assert the struct field offsets 
    _Static_assert(offsetof(struct frame_header, magic) == 0, "expected magic offset to be 0");
    _Static_assert(offsetof(struct frame_header, sequence_id) == 8, "expected sequence_id offset to be 8");
    _Static_assert(offsetof(struct frame_header, flags) == 16, "expected flags offset to be 16");
    _Static_assert(offsetof(struct frame_header, checksum) == 18, "expected flags offset to be 18");
    _Static_assert(sizeof(struct frame_header) == 24, "expected size 24");
    _Static_assert(alignof(struct frame_header) == 8, "expected struct alignment to be 8");
    // calculate and asserting  members total bytes 
    size_t total_bytes = 0;
    size_t payload_bytes = 0;
    size_t tail_paddings = 0;
    size_t internal_paddings = 0;
    
    // Get the start and end range of each struct field and assert
    size_t magic_start = offsetof(struct frame_header, magic);
    size_t magic_end = magic_start + sizeof(uint8_t);
    assert(magic_start == 0);
    assert(magic_end == 1);
    
    size_t sequence_id_start = offsetof(struct frame_header, sequence_id);
    size_t sequence_id_end = sequence_id_start + sizeof(uint64_t);
    assert(sequence_id_start == 8);
    assert(sequence_id_end == 16);

    size_t flags_start = offsetof(struct frame_header, flags);
    size_t flags_end = flags_start + sizeof(uint8_t);
    assert(flags_start == 16);
    assert(flags_end == 17);

    size_t checksum_start = offsetof(struct frame_header, checksum);
    size_t checksum_end = offsetof(struct frame_header, checksum) + sizeof(uint16_t);
    assert(checksum_start == 18);
    assert(checksum_end == 20);

    for (uint8_t i = 0; i < sizeof(struct frame_header); i++) {
        const uint8_t *raw_byte = (const uint8_t *)hdr + i;
        
        if (i >= magic_start && i < magic_end) {
            payload_bytes += 1;
            printf("Offset %2hhu | Addr %p | Value: 0x%02X | [magic]\n",i, (void *)raw_byte, *raw_byte);
        } else if (i >= sequence_id_start && i < sequence_id_end) {
            payload_bytes += 1;
            printf("Offset %2hhu | Addr %p | Value: 0x%02X | [sequence_id]\n",i, (void *)raw_byte, *raw_byte);
        } else if (i >= flags_start && i < flags_end) {
            payload_bytes += 1;
            printf("Offset %2hhu | Addr %p | Value: 0x%02X | [flags]\n",i, (void *)raw_byte, *raw_byte);
        } else if (i >= checksum_start && i < checksum_end) {
            payload_bytes += 1;
            printf("Offset %2hhu | Addr %p | Value: 0x%02X | [checksum]\n",i, (void *)raw_byte, *raw_byte);
        } else if (i >= checksum_end) {
            tail_paddings += 1;
            printf("Offset %2hhu | Addr %p | Value: 0x%02X | [TAIL_PADDING]\n",i, (void *)raw_byte, *raw_byte);
        } else {
            internal_paddings += 1;
            printf("Offset %2hhu | Addr %p | Value: 0x%02X | [INTERNAL_PADDING]\n",i, (void *)raw_byte, *raw_byte);
        }   
    }

    printf("--------------------------------------------------------------------------------\n");  

    assert(payload_bytes == 12);
    assert(internal_paddings == 8);
    assert(tail_paddings == 4);
    
    total_bytes = payload_bytes + internal_paddings + tail_paddings;
    assert(total_bytes == 24);

    struct byte_inspection_summary dummy;
    dummy.total_bytes_inspected = total_bytes;
    dummy.payload_bytes_count = payload_bytes;
    dummy.internal_pad_count = internal_paddings;
    dummy.tail_pad_count = tail_paddings;
    return dummy;
}
