#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define MAX_CAPACITY 3

typedef struct
{
    /**
     * bit 7  : is_flying
     * bit 4-6: batter_level
     * bit 0-3: error_code 
     */
    uint16_t drone_id; 
    uint8_t is_flying;
    uint8_t batter_level;
    uint8_t error_code;
    uint8_t temperature;
    /**
     * Law II — Struct-Level Alignment
     *
     * 6 bytes in total
     * max(2,1,1,1, 1) = 2 
     * 6 % 2 = 0 
     *
     * Law III — Tail Padding (The Array Stride Rule)
     *
     * Last member is temperature
     * sizeof(Packet) - (offsetof(temperature) + sizeof(temperature))
     * 6 - (5 + 1) = 0 
     */
} Packet;

/**
 * @brief Helper struct used to store the Packet buffer 
 * and the result code of its instantiation. 
 */
typedef struct
{
    Packet **buffer;
    int response_code; 
} BufferAllocation;

/**
 * @brief Helper struct used to store the parsed packet  
 */
typedef struct
{
    Packet packet;
    int response_code;
} ParsedPacket;

/**
 * @brief Allocate the space for a double buffer pointer
 * @param elements size_t
 * @return BufferAllocation struct
 */
BufferAllocation init(size_t elements);

/**
 * @brief Parses a single packet 
 *
 * @param stream uint8_t single drone 4 bytes packet
 * @return ParsedPacket struct containing the 
 * parsed Packet
 */
ParsedPacket parse_packet(uint8_t *stream);

/**
 * @brief Helper method used to assert the Packet's values
 * 
 * @param p Packet
 * @param id uint8_t
 * @param is_flying uint8_t
 * @param batter_level uint8_t
 * @param error_code uint8_t
 * @param temperature uint8_t
 * @return void
 */
void assert_data(Packet p, uint16_t id, uint8_t is_flying, uint8_t batter_level, uint8_t error_code, uint8_t temperature);

/**
 * @brief Stores the parsed packet into the buffer 
 *
 * @param buffer Packet double pointer
 * @param packet Packet
 * @param index size_t
 * @return int
 */
int store_parsed_packet(Packet **buffer, Packet packet, size_t index);

/**
 * @brief Helper function used to compare in ASC 
 * two void pointers
 *
 * @param a const void pointer
 * @param b const void pointer
 * @return int
 */
int compare_asc(const void *a, const void *b);

// struct declaration for the function pointers
struct SortingPacket;

// Function pointers signature for packets sorting 
typedef void (*SortPackets)(struct SortingPacket *self, size_t num_packtes, size_t size, int(*compare)(const void *a, const void *b));

/**
 * @brief Helper struct used to handle the sorting of 
 * parsed packets
 */
typedef struct SortingPacket
{
    Packet **buffer;
    SortPackets sorting;
} SortingPacket;

/**
 * @brief Sorts Packet from the buffer using a compare callback 
 *
 * @param self SortingPacket struct
 * @param num_packtes size_t
 * @param size size_t
 * @param compare int pointer function callback
 * @return void
 */
void sorting(SortingPacket *self, size_t num_packtes, size_t size, int(*compare)(const void *a, const void *b));

/**
 * @brief Helper function used to check the sorted buffer 
 *
 * @param buffer Packet double pointer
 * @param num_packtes size_t
 * @return void
 */
void print_stored_packet(Packet **buffer, size_t num_packtes);

/**
 * @brief Cleans up allocated data 
 *
 * @param buffer Packet double pointer
 * @param num_packtes size_t
 * @return void
 */
void cleanup(Packet **buffer, size_t num_packtes);

/// MAIN METHOD -------------------------------------------------------------------------------------------------------

int main(void)
{
    // Mock network data: 3 drones sending 4 bytes each.
    // Drone A: ID=0x1234, Flying=1, Batt=5, Err=2, Temp=30 (0x34 = 0011 0100 -> Wait, 1_101_0010 = 0xD2)
    uint8_t stream1[4] = { 0x12, 0x34, 0xD2, 30 };
    // Drone B: ID=0x00FF, Flying=0, Batt=7, Err=0, Temp=25 (0_111_0000 = 0x70)
    uint8_t stream2[4] = { 0x00, 0xFF, 0x70, 25 };
    // Drone C: ID=0x4321, Flying=1, Batt=1, Err=15, Temp=40 (1_001_1111 = 0x9F)
    uint8_t stream3[4] = { 0x43, 0x21, 0x9F, 40 };
    
    BufferAllocation ba = init(MAX_CAPACITY);
    if (ba.response_code != 1) return -1;
    
    // Parsing and Asserting values 
    ParsedPacket pp1 = parse_packet(stream1);
    if (pp1.response_code != 1) return -1;
    assert_data(pp1.packet, 0x1234, 1, 5, 2, 30);

    ParsedPacket pp2 = parse_packet(stream2);
    if (pp2.response_code != 1) return -1;
    assert_data(pp2.packet, 0x00FF, 0, 7, 0, 25);
    
    ParsedPacket pp3 = parse_packet(stream3);
    if (pp3.response_code != 1) return -1;
    assert_data(pp3.packet, 0x4321, 1, 1, 15, 40);

    // Store the parsed packet to the buffer
    if (store_parsed_packet(ba.buffer, pp1.packet, 0) != 1) return -1;
    if (store_parsed_packet(ba.buffer, pp2.packet, 1) != 1) return -1;
    if (store_parsed_packet(ba.buffer, pp3.packet, 2) != 1) return -1;    
    
    // Sorting Packets 
    SortingPacket sp;
    sp.buffer = ba.buffer;
    sp.sorting = sorting;
    
    sp.sorting(&sp, MAX_CAPACITY, sizeof(Packet *), compare_asc);

    // Check the sorting result
    print_stored_packet(ba.buffer, MAX_CAPACITY);

    // cleanup
    cleanup(ba.buffer, MAX_CAPACITY);

    return 0;
}

/// IMPLEMENTATION ----------------------------------------------------------------------------------------------------

BufferAllocation init(size_t elements) {
    
    BufferAllocation ba; 

    if (elements <= 0)
    {
        ba.buffer = NULL;    
        ba.response_code = 0; 
        return ba;
    }
    
    Packet **buffer = (Packet **)calloc(elements, sizeof(Packet *));

    if (buffer == NULL)
    {
        ba.buffer = NULL;
        ba.response_code = 0;    
        return ba;
    }

    for (size_t i = 0; i < elements; i++) {
        // Allocate space for new Packet
        *(buffer + i) = calloc(1, sizeof(Packet));
        
        if (*(buffer + i) == NULL) {
            
            // rollback, free and return an error code  if the pointer is NULL
            // after the instantiation. 
            for (size_t j = 0; j < elements; j++) {
                if (*(buffer + j) != NULL) free(*(buffer + j));
            }
            
            free(buffer);
            ba.buffer = NULL;
            ba.response_code = 0;
            return ba;
        }
    }

    ba.buffer = buffer;
    ba.response_code = 1;

    return ba;
}

ParsedPacket parse_packet(uint8_t *stream) {
    
    ParsedPacket pp;

    if (stream == NULL) {
        pp.response_code = 0;
        return pp;
    }
    
    // create the stack Packet p
    Packet p;

    // Getting the values to compose the drone_id
    uint16_t id = (((uint16_t)*(stream + 0) << 0x8) | *(stream + 1));
    // Getting the values from the flags 
    uint8_t is_flying = (*(stream + 2) >> 0x7);
    uint8_t batter_level = (*(stream + 2) >> 0x4) & 0x7;
    uint8_t error_code = (*(stream + 2) & 0xF);
    
    // add values to the Packet
    p.drone_id = id; 
    p.is_flying = is_flying;
    p.batter_level = batter_level;
    p.error_code = error_code;
    p.temperature = *(stream + 3);

    // add values to the the ParsedPacket
    pp.packet = p;
    pp.response_code = 1;

    return pp;
}

void assert_data(Packet p, uint16_t id, uint8_t is_flying, uint8_t batter_level, uint8_t error_code, uint8_t temperature) {
    
    assert(p.drone_id == id);
    assert(p.is_flying == is_flying);
    assert(p.batter_level == batter_level);
    assert(p.error_code == error_code);
    assert(p.temperature == temperature); 
}


int store_parsed_packet(Packet **buffer, Packet packet, size_t index) {

    Packet *np = *(Packet **)(buffer + index);
    if (np == NULL) return 0;

    np->drone_id = packet.drone_id;
    np->is_flying = packet.is_flying;
    np->batter_level = packet.batter_level;
    np->error_code = packet.error_code;
    np->temperature = packet.temperature;

    printf("Stored parsed packet id [0x%X] at address: %p\n", np->drone_id, (void *)np);
    return 1; 
}

void sorting(SortingPacket *self, size_t num_packtes, size_t size, int(*compare)(const void *a, const void *b)) {
    qsort((void **)self->buffer, num_packtes, size, compare);
}

int compare_asc(const void *a, const void *b) {
    // Cast to double pointer, then dereference once to get the Packet pointer
    const Packet *p1 = *(const Packet **)a;
    const Packet *p2 = *(const Packet **)b;
    // desc order
    return p2->batter_level - p1->batter_level;
}

void print_stored_packet(Packet **buffer, size_t num_packtes) {
    
    printf("\n .:: Drones Report ::.\n");
    printf("---------------------------------\n");
    
    for (size_t i = 0; i < num_packtes; i++) {
        Packet *p = *((Packet **)(buffer + i));
        printf("DRONE %zu DATA:\n", i);
        printf("Drone id     : 0x%X\n", p->drone_id);
        printf("Battery Level: %d\n", p->batter_level);
        printf("Is Flying    : %s\n", (p->is_flying == 0 ? "NO" : "YES"));
        printf("Error code   : %d\n", p->error_code);
        printf("Temerature   : %d\n", p->temperature);
        printf("---------------------------------\n");
    }
}

void cleanup(Packet **buffer, size_t num_packtes) {
    
    if (buffer == NULL) return;

    for (size_t i = 0; i < num_packtes; i++) {
        Packet *p = *((Packet **)(buffer + i));
        if (p != NULL) free(p);
    }

    free(buffer);
}
