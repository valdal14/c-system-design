#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define POOL_SIZE 1024
#define BLOCK_SIZE 16
#define MIN_BLOCK_SIZE 8

typedef struct FreeNode {
    struct FreeNode *next;
} FreeNode;

typedef struct {
    uint8_t *buffer;
    size_t pool_size;
    size_t block_size;
    FreeNode *head;
} Pool;

/**
 * @brief Init the Pool with a fixed size and with a fixed block size 
 * @param p Pool double pointer
 * @param pool_size size_t
 * @param block_size size_t
 * @return void
 */
void pool_init(Pool **p, size_t pool_size, size_t block_size);

/**
 * @brief Alloc a new block on the Pool
 * @param p Pool pointer
 * @return void pointer
 */
void *alloc_block(Pool *p);

/**
 * @brief Validated the given block size that must be min 8 or max 16 bytes.
 * Returns 1 if valid 0 if not
 *
 * param block_size uint8_t
 * @return uint8_t 
 */
uint8_t validate(uint8_t block_size) {
    return block_size == MIN_BLOCK_SIZE || block_size == BLOCK_SIZE ? 1 : 0;
}

/// MAIN METHOD ///////////////////////////////////////////////////////////////////////////

int main(void) {
    Pool *pool = NULL;
    pool_init(&pool, POOL_SIZE, BLOCK_SIZE);
    printf("Pool allocated at address %p\n", (void *)pool);

    void *record = alloc_block(pool);

    if (record == NULL) {
        fprintf(stderr, "Invalid block allocation\n");
    } else {
        uint8_t *data = (uint8_t *)((char *)record);
        *data = 14;
    }

    return 0;
}

/// IMPLEMENTATION ////////////////////////////////////////////////////////////////////////

void pool_init(Pool **p, size_t pool_size, size_t block_size) {
    
    if (validate(block_size) == 0) {
        fprintf(stderr, "Invalid block size\n");
        exit(EXIT_FAILURE);
    }

    Pool *mem_pool = (Pool *)calloc(1, sizeof(Pool));
    if (mem_pool == NULL) exit(EXIT_FAILURE);

    mem_pool->pool_size = pool_size;
    mem_pool->block_size = block_size;

    mem_pool->buffer = (uint8_t *)calloc(1, sizeof(char) * pool_size);
    if (mem_pool->buffer == NULL) exit(EXIT_FAILURE);

    uint8_t chunks = pool_size / block_size;
    
    for (uint8_t i = 0; i < chunks; i++) {
        void *chunk = mem_pool->buffer + (i * block_size);
        void *next_chunk = mem_pool->buffer + ((i + 1) * block_size);
        FreeNode *node = (FreeNode *)chunk;

        if (i == chunks - 1) {
            node->next = NULL; 
        } else {
            node->next = (FreeNode *)next_chunk;
        }
    }

    mem_pool->head = (FreeNode *)mem_pool->buffer;

    *p = mem_pool;
}

   
void *alloc_block(Pool *p) {
    
    if (p->head == NULL) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }

    // Grab the first available block
    FreeNode *allocated_block = p->head;

    // Read its address to find the next available block, and update the pool's head.
    p->head = allocated_block->next;

    return (void *)allocated_block;
}
