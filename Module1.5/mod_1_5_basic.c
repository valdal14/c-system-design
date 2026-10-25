#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Comparator to sort uint32_t in ascending order.
 */
int compare_asc(const void *a, const void *b);

/**
 * @brief Comparator to sort uint32_t in descending order.
 */
int compare_desc(const void *a, const void *b);

/**
 * @brief Helper to print the array.
 */
void print_array(const uint32_t *arr, size_t size);

/**
 * @brief Sort the given array using qsort function and 
 * the sorting is performed using the SortingType
 *
 * @param arr uint32_t pointer
 * @param size size_t
 * @param compare void pointer callback 
 * @return void
 */
void sorting(uint32_t *arr, size_t size, int(*compare)(const void *a, const void *b));

int main(void)
{
    uint32_t sensors[] = {44, 12, 99, 4, 76, 23};
    size_t size = sizeof(sensors) / sizeof(uint32_t);
    print_array(sensors, size);
    sorting(sensors, size, compare_asc);
    print_array(sensors, size);
    sorting(sensors, size, compare_desc);
    print_array(sensors, size);
    return 0;
}

void print_array(const uint32_t *arr, size_t size) {
    for (size_t i = 0; i < size; i++) {
        printf("sensor[%zu] = %u\n", i, *(arr + i));
    }
    printf("--------------\n");
}

void sorting(uint32_t *arr, size_t size, int(*compare)(const void *a, const void *b)) {
    qsort(arr, size, sizeof(uint32_t), compare);
}

int compare_asc(const void *a, const void *b) {
    uint32_t x = *(const uint32_t*)a;
    uint32_t y = *(const uint32_t*)b;
    
    if (x < y) return -1;
    if (x > y) return 1;
    return 0;
}

int compare_desc(const void *a, const void *b) {
    uint32_t x = *(const uint32_t*)a;
    uint32_t y = *(const uint32_t*)b;
    
    if (x > y) return -1;
    if (x < y) return 1;
    return 0;     
}
