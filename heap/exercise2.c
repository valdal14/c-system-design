#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct FreeNode {
    struct FreeNode *next;
} FreeNode;

int main(void) {
    uint8_t buffer[64];
    uint8_t block_size = 16;
    uint8_t chunks = 4;

    for (uint8_t i = 0; i < (block_size / chunks); i++) {
        void *chunk = (void *)((char *)buffer + (i * block_size));
        void *next_chunk = (FreeNode *)((char *)buffer + ((i + 1) * block_size));

        FreeNode *node = (FreeNode *)chunk;

        if (chunks - 1 == i) {
            node->next = NULL;
        } else {
            node->next = (FreeNode *)next_chunk;
        }
    }

    FreeNode *current_head = (FreeNode *)buffer;

    while (current_head != NULL) {
        printf("%p\n", (void *)current_head);
        current_head = current_head->next;
    }

    // Cast to FreeNode and set the head to be the
    // next available block 

    FreeNode *head = (FreeNode *)buffer;
    uint8_t counter = 2;

    while (head != NULL) { 
        FreeNode *safe_next = head->next;
        uint8_t *val = (uint8_t *)head;
        *val = counter * 2;
        head = safe_next;
        counter++;
    }

    for (uint8_t i = 0; i < (block_size / chunks); i++) {
        uint8_t *val = (uint8_t *)((char *)buffer + (i * block_size));
        printf("val[%d] = %d\n", i, *val);
    }
    

    return 0;
}
