#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
    uint8_t room[8] = {0};
    uint8_t block_size = 2;
    uint8_t chunks = (sizeof(room) / sizeof(room[0])) / block_size;
    char *pointers[chunks];
    char letters[4] = {'A', 'B', 'C', 'D'};

    for (uint8_t i = 0; i < chunks; i++) {
        char *p = (char *)room + (i * block_size);
        pointers[i] = p;
    }
 
    for (uint8_t i = 0; i < chunks; i++) {
        printf("%p\n", pointers[i]);
    }

    // use all the pointers to assign a room value
    for (uint8_t i = 0; i < chunks; i++) {
        char *p = pointers[i];
        *p = letters[i];
    }

    // now I am expecting the value changed in each room...
    // by either accessing the room directly or via the pointers 
    for (uint8_t i = 0; i < chunks; i++) {
        printf("room %c\n", room[i * block_size]);
    }

    return 0;

}
