#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

/* --- 1. THE DATA MODELS --- */
typedef struct Node {
    struct Node *prev;
    struct Node *next;
    uint16_t drone_id;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    size_t length;
} LRUList;

/* --- 2. PIPELINE SIGNATURES --- */

/**
 * @brief Init the LRUList 
 * @return LRUList pointer
 */
LRUList* lru_init(void);

/**
 * @brief Push a new value to the front of the Queue
 *
 * @param list LRUList pointer
 * @param drone_id uint16_t
 * @return void
 */
void push_front(LRUList *list, uint16_t drone_id);

/**
 * @brief Detach a Node from the LRUList
 *
 * @param list LRUList pointer
 * @param target Node pointer
 * @return void
 */
void detach_node(LRUList *list, Node *target);

/**
 * @brief Pop a Node from the tail of the LRUList 
 *
 * @param list LRUList pointer
 * return uint16_t
 */
uint16_t pop_tail(LRUList *list);

/**
 * @brief Free ups all allocated memory
 *
 * @param list LRUList pointer
 * @return void
 */
void cleanup(LRUList *list);

/* --- 3. THE TEST HARNESS --- */
int main(void) {
    LRUList *cache = lru_init();
    
    // 1. Load the Cache (Most recent goes to the front)
    push_front(cache, 0x1111); // State: [Head: 1111]
    push_front(cache, 0x2222); // State: [Head: 2222 <-> 1111 :Tail]
    push_front(cache, 0x3333); // State: [Head: 3333 <-> 2222 <-> 1111 :Tail]
    
    assert(cache->length == 3);
    assert(cache->head->drone_id == 0x3333);
    assert(cache->tail->drone_id == 0x1111);

    // 2. The LRU Cache Hit! We access the middle node (0x2222)
    // To update the LRU, we must detach it from the middle and push it to the front.
    Node *accessed_node = cache->head->next; // Grabbing 0x2222 directly
    assert(accessed_node->drone_id == 0x2222);
    
    detach_node(cache, accessed_node); 
    // State should now be: [Head: 3333 <-> 1111 :Tail]
    assert(cache->length == 2);
    assert(cache->head->next->drone_id == 0x1111);
   
    // Move it to the front (Simulating the cache update)
    push_front(cache, accessed_node->drone_id);
    free(accessed_node); // Free the old detached shell
    
    // State should now be: [Head: 2222 <-> 3333 <-> 1111 :Tail]
    assert(cache->head->drone_id == 0x2222);

    // 3. Cache is full! Evict the Least Recently Used (The Tail)
    uint16_t evicted_id = pop_tail(cache);
    assert(evicted_id == 0x1111);
    // State should now be: [Head: 2222 <-> 3333 :Tail]
    assert(cache->tail->drone_id == 0x3333);
    assert(cache->length == 2);

    printf("Phase 2.2 Doubly Linked LRU: PASSED\n");
    
    cleanup(cache);
   
    return 0;
}

LRUList* lru_init(void) {
    
    LRUList *list = (LRUList *)calloc(1, sizeof(LRUList));
    if (list == NULL) return NULL;

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;
    return list;
}

void push_front(LRUList *list, uint16_t drone_id) {
    
    Node *node = (Node *)calloc(1, sizeof(Node));

    if (list == NULL || node == NULL) return;

    node->drone_id = drone_id;

    if (list->head == NULL)
    {
        list->head = node;
        list->tail = node;
    }
    else
    {
        Node *old_head = list->head;
        // The new node looks FORWARD at the old head
        node->next = old_head;
        // The new node looks BACKWARD at NULL being the new head 
        node->prev = NULL;
        // The old head looks BACKWARD at the new node
        old_head->prev = node;
        // Update the main list pointer
        list->head = node;
    }

    list->length += 1;
}

void detach_node(LRUList *list, Node *target) {
    if (list == NULL || target == NULL || list->length == 0) return;

    // Wire the LEFT neighbor (or update the Head if target has no left neighbor)
    if (target->prev != NULL) {
        target->prev->next = target->next;
    } else {
        list->head = target->next; 
    }

    // Wire the RIGHT neighbor (or update the Tail if target has no right neighbor)
    if (target->next != NULL) {
        target->next->prev = target->prev;
    } else {
        list->tail = target->prev; 
    }

    list->length -= 1;
}

uint16_t pop_tail(LRUList *list) {

    if (list == NULL || list->length == 0) return 0;
    
    Node *old_tail = list->tail;
    uint16_t val = old_tail->drone_id; 

    list->tail = old_tail->prev;

    // Safely cut the wire, or register the list as completely empty
    if (list->tail != NULL) {
        list->tail->next = NULL; 
    } else {
        list->head = NULL; 
    }
    
    free(old_tail);
    list->length -= 1;
    
    return val;
}

void cleanup(LRUList *list) {
    if (list == NULL) return;

    Node *current = list->head;

    while (current != NULL) {
        Node *next_node = current->next;
        free(current);
        current = next_node;
    }

    free(list);
}
