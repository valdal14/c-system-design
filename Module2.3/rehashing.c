#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define MAX_SIZE 254

/* --- 1. THE DATA MODELS --- */
struct HashTable;
struct HashNode;

typedef void (*HashInsert)(struct HashTable *self, uint16_t key, uint8_t value, uint8_t(*hash)(uint16_t key, size_t size));
typedef void (*HashResize)(struct HashTable *self, uint8_t(*hash)(uint16_t key, size_t size));
typedef uint8_t (*HashLookup)(struct HashTable *self, uint16_t key, uint8_t *found, uint8_t(*hash)(uint16_t key, size_t size));
typedef void (*HashAddNew)(struct HashTable *self, struct HashNode *node, uint8_t index);

/**
 * @brief Helper function used to compute the 
 * index of the table based of the given key 
 * and the size of the buffer 
 *
 * @param key uint16_t 
 * @param size size_t
 * @return uint8_t: -1 if the given size is greater than
 * the current HashTable's size 
 */
uint8_t hash_function(uint16_t key, size_t size);

typedef struct HashNode {
    uint16_t key;
    uint8_t value;
    struct HashNode *next;
} HashNode;

typedef struct HashTable {
    HashNode **buffer;
    HashInsert insert;
    HashResize resize;
    HashLookup lookup;
    HashAddNew add;
    size_t size;  // Total buckets
    size_t count; // Total nodes inserted
} HashTable;

/* --- 2. PIPELINE SIGNATURES --- */

/**
 * @brief Inits a new HashTable
 *
 * @param ht HashTable double pointer
 * @param size_t initial_size
 * @return HashTable struct
 */
void hash_init(HashTable **ht, size_t initial_size);

/**
 * @brief Inser a new HashNode in the HashTable's buffer 
 *
 * @param self HashTable pointer
 * @param key uint16_t 
 * @param value uint8_t
 * @param uint8_t callback that computes the hashing algorithm
 * @return void
 */
void hash_insert(HashTable *self, uint16_t key, uint8_t value, uint8_t(*hash)(uint16_t key, size_t size));

/**
 * @brief Resise the current HashTable's buffer 
 * 
 * @param self HashTable struct pointer
 * @param uint8_t callback that computes the hashing algorithm
 * @param return void
 */
void hash_resize(HashTable *self, uint8_t(*hash)(uint16_t key, size_t size));

/**
 * @brief Performs HashNode lookup 
 * 
 * @param self HashTable struct pointer
 * @param key uint16_t
 * @param found uint8_t pointer
 * @param uint8_t callback that computes the hashing algorithm
 * @return uint8_t: Change the found pointer to 1 if found 
 * and returns the value of the HashNode based on the given key 
 */
uint8_t hash_lookup(HashTable *self, uint16_t key, uint8_t *found, uint8_t(*hash)(uint16_t key, size_t size));

/**
 * @brief Helper function used to add a new node
 *
 * @param self HashTable struct pointer
 * @param node HashNode pointer
 * @param uint8_t index
 * @return void
 */
void add_new(struct HashTable *self, struct HashNode *node, uint8_t index);

/**
 * @brief Deallocate all allocated memory
 *
 * @param ht HashTable struct pointer
 * @return void
 */
void hash_free(HashTable *ht);

/* --- 3. THE TEST HARNESS --- */
int main(void) {
    // 1. Initialize a tiny table to easily trigger a resize
    HashTable *ht = NULL;
    size_t size = 2;
    hash_init(&ht, size);
    assert(ht->size == size);
    assert(ht->count == 0);

    // 2. Insert keys. 
    // 0x1111 (4369) % 2 = 1
    // 0x2222 (8738) % 2 = 0
    ht->insert(ht, 0x1111, 35, hash_function);
    ht->insert(ht, 0x2222, 42, hash_function);
    
    assert(ht->count == 2);
    assert(ht->size == 2); // Load factor is exactly 1.0 (2/2)

    // 3. The Trigger: Inserting a 3rd item exceeds the 1.0 load factor.
    // Inside hash_insert, you must call hash_resize(ht) BEFORE adding this node.
    ht->insert(ht, 0x3333, 99, hash_function); 
    
    // 4. Assert the Reallocation
    assert(ht->size == 4);   // The table should have doubled
    assert(ht->count == 3);
    
    // 5. Assert the Migration was mathematically correct
    // 0x1111 (4369) % 4 = 1
    // 0x2222 (8738) % 4 = 2 (Moved from bucket 0 to bucket 2!)
    // 0x3333 (13107) % 4 = 3
   
    uint8_t found = 0;
    assert(ht->lookup(ht, 0x1111, &found, hash_function) == 35 && found == 1);
    assert(ht->lookup(ht, 0x2222, &found, hash_function) == 42 && found == 1);
    assert(ht->lookup(ht, 0x3333, &found, hash_function) == 99 && found == 1);

    // Ensure the old bucket 0 is now empty because 0x2222 moved to bucket 2
    assert(ht->buffer[0] == NULL); 
    assert(ht->buffer[2]->key == 0x2222);

    // 6. Teardown
    hash_free(ht);
    printf("Phase 2.3 Blocking Resize: PASSED\n"); 
 
    return 0;
}

/* --- 4. IMPLEMENTATION --- */

uint8_t hash_function(uint16_t key, size_t size) {
    if (size > MAX_SIZE) return -1;
    return key % size;
}

void hash_init(HashTable **ht, size_t initial_size) {
    if (ht == NULL || initial_size > MAX_SIZE) return;

    HashTable *new_ht = (HashTable *)calloc(1, sizeof(HashTable));
    if (new_ht == NULL) return;

    new_ht->buffer = (HashNode **)calloc(initial_size, sizeof(HashNode *));
    if (new_ht->buffer == NULL) return;
    
    new_ht->insert = hash_insert;
    new_ht->resize = hash_resize;
    new_ht->lookup = hash_lookup;
    new_ht->size = initial_size;
    new_ht->add = add_new;
    new_ht->count = 0;

    *ht = new_ht;
}

void hash_insert(HashTable *self, uint16_t key, uint8_t value, uint8_t(*hash)(uint16_t key, size_t size)) {
    
    if (self == NULL) return;

    /**
     * Note: If the load factor of the HashTable
     * is 1 a rehashing will be applied. 
     */
    if ((self->count / self->size) == 1) hash_resize(self, hash);
    
    // Check if Load Factor >= 0.75
    if ((self->count * 4) >= (self->size * 3)) hash_resize(self, hash);

    HashNode *hn = (HashNode *)calloc(1, sizeof(HashNode));
    if (hn == NULL) return;

    size_t idx = hash(key, self->size);
    hn->key = key;
    hn->value = value;
    // add the new HashNode
    self->add(self, hn, idx);
}

void hash_resize(HashTable *self, uint8_t(*hash)(uint16_t key, size_t size)) {

    // Keeping the old size value alive to free the old buffer 
    size_t old_size = self->size;
    // double the old_size 
    size_t new_size = old_size * 2;

    HashNode **new_buffer = (HashNode **)calloc(new_size, sizeof(HashNode *));
    if (new_buffer == NULL) return;

    for (size_t i = 0; i < self->size; i++)
    {
        // Get the current head at index i 
        HashNode *n = self->buffer[i];
        
        while (n != NULL)
        {
            // Save the chain before changing HashNode n 
            HashNode *next_node = n->next;
            // Calculate the new index 
            size_t new_idx = hash(n->key, new_size);
            // move the node to the new buffer 
            n->next = new_buffer[new_idx];
            new_buffer[new_idx] = n;

            // move forward in the LL
            n = next_node;
        }
    }

    // free the old buffer 
    free(self->buffer);
    // assign the new buffer to the HashTable
    self->buffer = new_buffer;
    self->size = new_size;
}

uint8_t hash_lookup(HashTable *self, uint16_t key, uint8_t *found, uint8_t(*hash)(uint16_t key, size_t size)) {

    if (self == NULL || found == NULL)
    {
        *found = 0;
        return *found;
    }

    size_t index = hash(key, self->size);
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

void add_new(struct HashTable *self, struct HashNode *node, uint8_t index) {
    if (self == NULL || node == NULL) return;

    // New node points to the current head (or NULL)
    node->next = self->buffer[index];
    // The bucket array points to the new node
    self->buffer[index] = node;
    self->count += 1;
}

void hash_free(HashTable *ht) {
    if (ht == NULL) return;

    for (size_t i = 0; i < ht->size; i++) {
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
