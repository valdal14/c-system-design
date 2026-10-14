#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct FreeNode {
    struct FreeNode *next;
} FreeNode;

int main(void) {
    uint8_t buffer[16];

    FreeNode *b0 = (FreeNode *)((char *)buffer);
    FreeNode *b1 = (FreeNode *)((char *)buffer + 8);
    b0->next = b1;
    b1->next = NULL;

    FreeNode *current = b0;
    uint8_t counter = 0;
    
    while (current != NULL) {
        printf("b[%d] address is: %p\n", counter, (void *)current);
        counter++;
        current = current->next;
    }

    // ALLOCATION 
    // We need to get the head first 
    FreeNode *head = b0;

    // Allocation 1 
    //
    // Starting from the head 
    FreeNode *user1 = head;
    // set the head to be the next before it get destoyed 
    head = user1->next;
    // cast and dereference to write the value at that allocated location 
    uint8_t *user_data_1 = (uint8_t *)user1;
    *user_data_1 = 14;


    // Allocation 2
    //
    // Starting from the head 
    FreeNode *user2 = head;
    // set the head to be the next before it get destoyed 
    head = user2->next;
    // cast and dereference to write the value at that allocated location 
    uint8_t *user_data_2 = (uint8_t *)user2;
    *user_data_2 = 127;

    printf("val1 = %d\n", *user_data_1); // Prints 14
    printf("val2 = %d\n", *user_data_2); // Prints 21


    return 0;
}
