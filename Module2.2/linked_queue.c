#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

/* --- 1. THE DATA MODELS --- */
typedef struct Node 
{
    struct Node *next;
    uint16_t drone_id;
} Node;

typedef struct
{
    Node *head;
    Node *tail;
    size_t length;
} Queue;


/* --- 2. PIPELINE SIGNATURES --- */

/**
 * @brief Inits a Queue 
 */
Queue* queue_init(void);

/**
 * @brief Enqueue a new Node in the Queue
 *
 * @param q Queue pointer
 * @param drone_id uint16_t
 * @return void
 */
void queue_enqueue(Queue *q, uint16_t drone_id);

/**
 * @brief Dequeue a Node from the Queue
 *
 * @param q Queue pointer
 * @return uint16_t
 */
uint16_t queue_dequeue(Queue *q);

/**
 * @brief Free the allocated heap 
 *
 * @param q Queue pointer
 * @return void
 */
void queue_free(Queue *q);

/* --- 3. THE TEST HARNESS --- */
int main(void) {
    Queue *q = queue_init();
    assert(q != NULL);
    assert(q->length == 0);

    // 1. Ingest arriving drone packets (Enqueue at Tail)
    queue_enqueue(q, 0x1111); // First in
    queue_enqueue(q, 0x2222);
    queue_enqueue(q, 0x3333); // Last in
    
    assert(q->length == 3);
    assert(q->head->drone_id == 0x1111);
    assert(q->tail->drone_id == 0x3333);

    // 2. Process packets (Dequeue from Head)
    uint16_t processed_id = queue_dequeue(q);
    assert(processed_id == 0x1111); // First out!
    assert(q->length == 2);
    assert(q->head->drone_id == 0x2222); // Head pointer successfully moved

    // 3. Teardown
    queue_free(q);
    printf("Phase 2.2 Linked Queue: PASSED\n");
    
    return 0;
}

/* --- 4. IMPLEMENTATION --- */
Queue* queue_init(void) {
    
    Queue *queue = (Queue *)calloc(1, sizeof(Queue));
    
    if (queue == NULL)
    {
        fprintf(stderr, "There was an error allocating the Queue\n");
        exit(EXIT_FAILURE);
    }
    
    queue->head = NULL;
    queue->tail = NULL;
    queue->length = 0;
    
    return queue;
}

void queue_enqueue(Queue *q, uint16_t drone_id) {
    
    if (q == NULL) 
    {
        fprintf(stderr, "Queue is NULL, the application will be terminated\n");
        exit(EXIT_FAILURE);
    }
    
    Node *new_node = (Node *)calloc(1, sizeof(Node));
    new_node->drone_id = drone_id;
    new_node->next = NULL;

    if (new_node == NULL) 
    {
        fprintf(stderr, "There was an error allocating the new Node\n");
        exit(EXIT_FAILURE);
    }

    if (q->head == NULL) 
    {
        q->head = new_node;
        q->tail = new_node;
    } 
    else 
    {
        // tail next point to the new node 
        q->tail->next = new_node;
        // set the tail to be the last (new node)
        q->tail = new_node;
    }

    // increase the length of the queue
    q->length += 1;
}

uint16_t queue_dequeue(Queue *q) {
    
    if (q == NULL)
    {
        fprintf(stderr, "Queue is NULL, the application will be terminated\n");
        exit(EXIT_FAILURE);
    }
    
    uint16_t drone_id = q->head->drone_id;
    // Save the old head
    Node *old_head = q->head;       
    // Move the queue forward
    q->head = q->head->next;         
    // Destroy the old head
    free(old_head);                  
    q->length -= 1;   
    
    return drone_id;
}

void queue_free(Queue *q) {
    if (q == NULL) return;

    Node *current = q->head;

    while (current != NULL) {
        Node *next_node = current->next; 
        free(current);                 
        current = next_node;          
    }

    free(q); 
}
