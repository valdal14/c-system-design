#include <assert.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Represents an unoptimized pipeline ingestion record in RAM.
 *
 * WARNING: Do not modify the member order or types of this struct.
 * It serves as the baseline target for your byte-level layout audit.
 */
struct suboptimal_record {
    /**
     * Law I — Strict Declaration Order & Member Alignment
     *
     * 1 byte type
     * 0 (offset) mod 1 (byte) = 0 (OK)
     * no padding needed
     */
    uint8_t  status_code;    
    /*
     * 8 bytes type
     * 1 (offset) mode 8 (bytes) = 1 (Paddings needed for alignment)
     * (1 + 8) + 7 (7 paddings to get to 16 since uint64_t needs to be divible by 8)
     * new offset start at 8
     */
    uint64_t record_id; 
    /*
     * 2 bytes type 
     * 16 (offset) mode 2 (bytes) = 0 (OK)
     * no padding needed
     */
    uint16_t partition_key;  
    /*
     * 8 bytes type
     * 18 (offset) mode 8 (bytes) = 2 (Paddings needed for alignment)
     * (18 + 6 (paddings)) + 8 (type) = 32 (6 paddings to get to 32 since uint64_t needs to be divible by 8)
     * new offset 24
     */
    double   metric_value;
    /*
     * 1 byte type 
     * 32 (offset) mode 8 (byte) = 0 (OK)
     * no padding needed
     */
    uint8_t  is_dirty;  
    /*
     * 4 bytes type
     * 33 (offset) mode 4 (bytes) = 1 (Paddings needed for alignment)
     * Since 33 mode 4 is not zero the compiler will add the 3 bytes needed to get to 40, the closer
     * value that will allow to return 0. So the real offset of batch_epoch is:
     *
     * 33 (current offset) + 3 (padding) = 36
     * 36 (offset) + 4 (type size) = 40 
     */
    uint32_t batch_epoch;
    /*
     * Law II — Struct-Level Alignment
     *
     * in total I am counting 40 bytes 
     * max(1, 8, 2, 4) = 8 bytes uint64_t type is the max 
     * 40 mod 8 = 0 (OK in theory since the struct is not obptimised according to law 1) 
     *
     * Law III — Tail Padding (The Array Stride Rule)
     *
     * NOTE: last member here is the field batch_epoch
     * sizeof(struct suboptimal_record) - (offset(last member) + sizeof(last member))
     * 
     * 40 (size of suboptimal_record) - (36 (offset) + 4 (sizeof batch_epoch)) = 0 
     */
};

/**
 * @brief Represents the cache-optimized layout of the pipeline ingestion record.
 */
struct optimal_record {
    uint64_t record_id; // offset 0 alignment 8
    double  metric_value; // offset 8 alignment 8
    uint32_t batch_epoch; // offset 16 alignment 4 
    uint16_t partition_key; // offset 20 alignment 2 
    uint8_t status_code; // offset 22 alignment 1 
    uint8_t is_dirty; // offset 23 alignment 1 
};

/**
 * @brief Captures comparative memory layout metrics between two struct layouts.
 */
struct layout_audit_report {
    size_t suboptimal_size;       /**< sizeof(struct suboptimal_record) in bytes */
    size_t suboptimal_alignment;  /**< alignof(struct suboptimal_record) in bytes */
    size_t optimal_size;          /**< sizeof(struct optimal_record) in bytes */
    size_t optimal_alignment;     /**< alignof(struct optimal_record) in bytes */
    size_t payload_bytes;         /**< Sum of raw member sizes without any padding */
    size_t suboptimal_waste_per_record; /**< Total padding bytes in suboptimal_record */
    size_t optimal_waste_per_record;    /**< Total padding bytes in optimal_record */
    size_t total_ram_saved_bytes; /**< Bytes saved across `record_count` instances */
    // Third law tail padding 
    size_t suboptimal_record_tail_padding; 
    size_t optimal_record_tail_padding;
};

/**
 * @brief Computes the memory footprint and padding savings between the suboptimal
 *        and optimal record layouts across a specified number of records.
 *
 * @param record_count The number of struct instances allocated in the pipeline array.
 * @return A fully populated `struct layout_audit_report` containing sizes,
 *         alignments, per-record padding waste, and total RAM bytes saved.
 */
struct layout_audit_report audit_struct_savings(size_t record_count);

int main(void) {
    struct layout_audit_report report = audit_struct_savings(1000);

    // suboptimal_record
    _Static_assert(offsetof(struct suboptimal_record, status_code) == 0, "status_code must start at offset 0");
    _Static_assert(offsetof(struct suboptimal_record, record_id) == 8, "record_id must start at offset 8");
    _Static_assert(offsetof(struct suboptimal_record, partition_key) == 16, "partition_key must start at offset 16");
    _Static_assert(offsetof(struct suboptimal_record, metric_value) == 24, "metric_value must start at offset 32");
    _Static_assert(offsetof(struct suboptimal_record, is_dirty) == 32, "is_dirty must start at offset 32");
    _Static_assert(offsetof(struct suboptimal_record, batch_epoch) == 36, "batch_epoch must start at offset 36");
    _Static_assert(alignof(struct suboptimal_record) == 8, "alignment should be 8 bytes");

    printf("----- SUBOPTIMAL_RECORD:\n");
    printf("suboptimal_record size : %zu\n", report.suboptimal_size);
    printf("suboptimal_record align: %zu\n", report.suboptimal_alignment);
    printf("Third Law (Tail padding) %zu\n", report.suboptimal_record_tail_padding);
    
    // optimal_record 
    _Static_assert(offsetof(struct optimal_record, record_id) == 0, "record_id must start at offset 0");
    _Static_assert(offsetof(struct optimal_record, metric_value) == 8, "metric_value must start at offset 8");
    _Static_assert(offsetof(struct optimal_record, batch_epoch) == 16, "batch_epoch must start at offset 16");
    _Static_assert(offsetof(struct optimal_record, partition_key) == 20, "partition_key must start at offset 20");
    _Static_assert(offsetof(struct optimal_record, status_code) == 22, "status_code must start at offset 22");
    _Static_assert(offsetof(struct optimal_record, is_dirty) == 23, "is_dirty must start at offset 23");
    _Static_assert(alignof(struct optimal_record) == 8, "alignment should be 8 bytes");

    printf("----- OPTIMAL_RECORD:\n");
    printf("optimal_record size : %zu\n", report.optimal_size);
    printf("optimal_record align: %zu\n", report.optimal_alignment);
    printf("Third Law (Tail padding) %zu\n", report.optimal_record_tail_padding);
    printf("--------------------------\n");
    // savings
    printf("Total savings in bytes: %zu\n", report.total_ram_saved_bytes);
    
    assert(report.total_ram_saved_bytes == 16000);

    return 0;
}

struct layout_audit_report audit_struct_savings(size_t record_count) { 
    
    struct suboptimal_record sub_dummy;
    size_t sum = (sizeof(sub_dummy.record_id) 
        + sizeof(sub_dummy.metric_value) 
        + sizeof(sub_dummy.batch_epoch) 
        + sizeof(sub_dummy.partition_key)
        + sizeof(sub_dummy.status_code)
        + sizeof(sub_dummy.is_dirty)
    );

    size_t suboptimal_tail_padding = sizeof(struct suboptimal_record) - (offsetof(struct suboptimal_record, batch_epoch) + sizeof(uint32_t));
    size_t optimal_tail_padding = sizeof(struct optimal_record) - (offsetof(struct optimal_record, is_dirty) + sizeof(uint8_t));

    struct layout_audit_report report;
    report.suboptimal_size = sizeof(struct suboptimal_record);
    report.suboptimal_alignment = alignof(struct suboptimal_record);
    report.optimal_size = sizeof(struct optimal_record);
    report.optimal_alignment = alignof(struct optimal_record);
    report.payload_bytes = sum;
    report.suboptimal_waste_per_record = sizeof(struct suboptimal_record) - sum;
    report.optimal_waste_per_record = sizeof(struct optimal_record) - sum;
    report.total_ram_saved_bytes = (sizeof(struct suboptimal_record) - sizeof(struct optimal_record)) * record_count;
    report.suboptimal_record_tail_padding = suboptimal_tail_padding;
    report.optimal_record_tail_padding = optimal_tail_padding;
    return report;
}
