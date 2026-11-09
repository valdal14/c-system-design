#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define BUCKET_COUNT 5


/* --- 1. THE DATA MODELS --- */
typedef struct HashNode
{
    uint16_t key;
    uint8_t value;
    struct HashNode *next;
} HashNode;

// Function pointers signature for the HashTable functions
struct HashTable;
typedef size_t (*HashFunction)(struct HashTable *self, uint16_t key);
typedef void (*HashInsert)(struct HashTable *self, uint16_t key, uint8_t value);
typedef uint8_t (*HashLookup)(struct HashTable *self, uint16_t key, uint8_t *found);

typedef struct HashTable
{
    HashNode **buffer;
    HashFunction hash;
    HashInsert insert;
    HashLookup lookup;
} HashTable;


/* --- 2. PIPELINE SIGNATURES --- */
/**
 * @brief Inits a new HashTable
 * @return HashTable pointer
 */
HashTable* hash_init(void);

/**
 * @brief Hash function 
 *
 * @param self HashTable struct 
 * @param key uint16_t
 * @return size_t (The index location)
 */
size_t hash_function(HashTable *self, uint16_t key);

/**
 * @brief Inserts a new value into the buffer
 *
 * @param self HashTable struct 
 * @param key uint16_t
 * @param value uint8_t
 * @return void
 */
void hash_insert(HashTable *self, uint16_t key, uint8_t value);

/**
 * @brief Performs lookup in the buffer
 *
 * @param self HashTable struct 
 * @param key uint16_t
 * @param found uint8_t pointer
 * @return uint8_t: 0 if not found otherwise the value found
 */
uint8_t hash_lookup(HashTable *self, uint16_t key, uint8_t *found);

/**
 * @brief Deallocate the previously allocated memory
 *
 * @param self HashTable struct 
 * @return void
 */
void hash_free(HashTable *ht);

/* --- 3. THE TEST HARNESS --- */
int main(void) {
    HashTable *ht = hash_init();
    assert(ht != NULL);

    // 1. Insert distinct keys
    // Let's assume bucket count is 5.
    // 0x1111 (4369) % 5 = 4
    // 0x2222 (8738) % 5 = 3
    ht->insert(ht, 0x1111, 35); 
    ht->insert(ht, 0x2222, 42); 
    
    // 2. Force a collision! 
    // 0x3333 (13107) % 5 = 2
    // 0x4444 (17476) % 5 = 1
    // 0x5555 (21845) % 5 = 0
    // 0x6666 (26214) % 5 = 4  <-- COLLISION WITH 0x1111!
    ht->insert(ht, 0x6666, 99); 
    
    // Both 0x1111 and 0x6666 are now in Bucket 4, chained together.

    // 3. Lookup Testing
    uint8_t found = 0;
    uint8_t val = 0;

    val = hash_lookup(ht, 0x1111, &found);
    assert(found == 1);
    assert(val == 35);

    
    val = hash_lookup(ht, 0x6666, &found);
    assert(found == 1);
    assert(val == 99); // The collision chain must successfully traverse to the right node
 
    // Lookup a non-existent key
    val = hash_lookup(ht, 0x9999, &found);
    assert(found == 0);

    // 4. Teardown
    hash_free(ht);
    printf("Phase 2.3 Hash Table: PASSED\n");
    
    return 0;
}

/* --- 4. IMPLEMENTATION --- */

HashTable* hash_init(void) {
    
    HashTable *ht = (HashTable *)calloc(1, sizeof(HashTable));
    if (ht == NULL) return NULL;
    
    ht->buffer = (HashNode **)calloc(BUCKET_COUNT, sizeof(HashNode *));
    if (ht->buffer == NULL) return NULL;
    
    ht->hash = hash_function;
    ht->insert = hash_insert;
    ht->lookup = hash_lookup;

    return ht;
}

size_t hash_function(HashTable *self, uint16_t key) {    
    if (self == NULL) return -1;
    (void)self; // unused self identifier
    return key % BUCKET_COUNT;
}

void hash_insert(HashTable *self, uint16_t key, uint8_t value) {
    
    if (self == NULL || value <= 0) return;
    
    size_t index = self->hash(self, key);
    if (index >= BUCKET_COUNT) return;

    // allocate a new node 
    HashNode *n = (HashNode *)calloc(1, sizeof(HashNode));
    if (n == NULL) return;
    
    n->key = key;
    n->value = value;
    // New node points to the current head (or NULL)
    n->next = self->buffer[index]; 
    // The bucket array points to the new node
    self->buffer[index] = n;       
}

uint8_t hash_lookup(HashTable *self, uint16_t key, uint8_t *found) {
    
    if (self == NULL || found == NULL)
    {
        *found = 0;
        return *found;
    }
    
    size_t index = self->hash(self, key);
    if (index >= BUCKET_COUNT)
    {
        *found = 0;
        return *found;
    } 

    // check first for an index out of bound
    HashNode *current = self->buffer[index];
    if (current == NULL)
    {
        *found = 0;
        return *found;
    }

    // return directly by key 
    if (current->key == key)
    {
        *found = 1;
        return self->buffer[index]->value;
    }
     
    // if not found directly by index start the LL lookup
    while (current != NULL)
    {
        if (current->key == key)
        {
            *found = 1;
            return current->value;
        }

        current = current->next;
    }

    // edge case return not found
    *found = 0; 
    return *found;
}

void hash_free(HashTable *ht) {
    if (ht == NULL) return;

    for (size_t i = 0; i < BUCKET_COUNT; i++) {
        HashNode *n = ht->buffer[i];
        while (n != NULL) {
            HashNode *next_node = n->next;
            free(n);
            n = next_node;
        }
    }
    
    free(ht->buffer); 
    free(ht);
}

