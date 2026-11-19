#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct Node {
    int packet_id;
    struct Node *next;
} Node;

Node* find_middle(Node *head) {
    if (head == NULL) return NULL;
    
    Node *s = head;
    Node *f = head;

    while (f != NULL && f->next != NULL)
    {
        s = s->next;
        f = f->next->next;
    }
   
    printf("Middle Node value: %d\n", s->packet_id);
    return s; 
}

// Helper to quickly build a list
Node* append(Node *head, int id) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->packet_id = id;
    new_node->next = NULL;
    if (!head) return new_node;
    Node *curr = head;
    while (curr->next) curr = curr->next;
    curr->next = new_node;
    return head;
}

int main(void) {
    Node *head = NULL;
    
    // Test 1: Odd number of nodes (1 -> 2 -> 3 -> 4 -> 5)
    // The middle should be exactly 3.
    for (int i = 1; i <= 5; i++) head = append(head, i);
    Node *mid_odd = find_middle(head);
    assert(mid_odd != NULL && mid_odd->packet_id == 3);
    
    // Test 2: Even number of nodes (1 -> 2 -> 3 -> 4 -> 5 -> 6)
    // The middle should be 4 (the second middle node).
    head = append(head, 6);
    Node *mid_even = find_middle(head);
    assert(mid_even != NULL && mid_even->packet_id == 4);
    
    printf("Drill 1.5: PASSED\n");
    return 0;
}
