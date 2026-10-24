#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PACKETS_NUM 3

/**
 * @brief DS helper to simulate the stream of 3 different packets
 */
typedef struct
{
    char *packets;
    size_t sizes[3];
    size_t offsets[3];
} Stream;

/**
 * @brief Allocates a 2D jagged array of uint32_t where each row can have a different length.
 *
 * @param num_rows   The total number of rows (streams). Must be > 0.
 * @param row_sizes  A 1D array containing the lengths of each row. Must not be NULL.
 * @return A double pointer (uint32_t **) to the fully allocated jagged array, or NULL on failure.
 */
uint32_t **alloc_jagged_array(size_t num_rows, const size_t *row_sizes);

/**
 * @brief Safely frees a 2D jagged array to prevent memory leaks.
 *
 * @param jagged_array The double pointer to the array. (If NULL, no-op).
 * @param num_rows     The number of rows in the array.
 */
void free_jagged_array(uint32_t **jagged_array, size_t num_rows);

/**
 * @brief Helpers function that returns a Stream DS for testing the 
 * allocation of a 2D array of pointers.
 *
 * @param ids char pointer array 
 * @param count size_t
 * @return Stream 
 */
Stream setup(char *ids[], size_t count);

/**
 * @brief Adds and encodes the data to the jagged array
 * by converting the char value to its ascii representation
 *
 * @param stream Stream
 * @param array uint32_t double pointer 
 * @param size_t count
 * @return void
 */
void encode_data(Stream stream, uint32_t **array, size_t count);

/**
 * @brief Decode and prints out the data from the jagged array
 * by converting the uint32_t into a row uint8_t and reassembling the 
 * string uisng the ascii representation of the numeric value 
 *
 * @param stream Stream
 * @param array uint32_t double pointer 
 * @param size_t count
 * @return void
 */
void decode_data(Stream stream, uint32_t **array, size_t count);

/// MAIN ------------------------------------------------------------------------------------------

int main(void)
{
    char *id_values[3] = {"BRS", "CFS1", "ESC23"};
    // prepare the data 
    Stream incoming_data = setup(id_values, PACKETS_NUM); 
    // allocate the jagged_array
    uint32_t **stagin_area = alloc_jagged_array(PACKETS_NUM, incoming_data.sizes);
    // decode the data 
    encode_data(incoming_data, stagin_area, PACKETS_NUM);
    // decode the data 
    decode_data(incoming_data, stagin_area, PACKETS_NUM);
    // free allocation
    free_jagged_array(stagin_area, PACKETS_NUM);
    return 0;
}

/// IMPLEMENTATION ---------------------------------------------------------------------------------

Stream setup(char *ids[], size_t count) {
    
    Stream stream; 
    size_t len = 0;

    for (size_t i = 0; i < count; i++) {
        len += strlen(*(ids + i)) + 1;
        *(stream.sizes + i) = strlen(*(ids + i)) + 1; 
    }

    *(stream.offsets + 0) = 0;
    *(stream.offsets + 1) = *(stream.sizes + 0);
    *(stream.offsets + 2) = *(stream.sizes + 0) + *(stream.sizes + 1);

    stream.packets = (char *)calloc(1, sizeof(char) * len);
    
    if (stream.packets == NULL) 
    {
        fprintf(stderr, "Error: Could not create the stream\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < count; i++) {
         char *str = (char *)((char *)stream.packets + *(stream.offsets + i));
        strncpy(str, *(ids + i), *(stream.sizes + i));
        str[*(stream.sizes + i) - 1] = '\0';
        printf("Added packet with ID: %s\n", stream.packets + *(stream.offsets + i));
    }
  
    return stream;
}


uint32_t **alloc_jagged_array(size_t num_rows, const size_t *row_sizes) {
   
    if (num_rows <= 0 || row_sizes == NULL)
    {
        fprintf(stderr, "Error: Invalid number or rows or sizes\n");
        exit(EXIT_FAILURE);
    }

    uint32_t **array = (uint32_t **)calloc(num_rows, sizeof(uint32_t *));

    if (array == NULL)
    {
        fprintf(stderr, "Error: Could not allocate staging area\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < num_rows; i++)
    {
        size_t *v = (size_t *)row_sizes + i;
        *(array + i) = (uint32_t *)calloc(1, sizeof(uint32_t) * (*v));
        
        if (*(array + i) == NULL)
        {
            fprintf(stderr, "Error: Could not allocated space for the row\n");
            // prevent memory leaks
            for (size_t j = 0; j < i; j++) {
                if (*(array + j) != NULL) free(*(array + j));
            }
            
            free(array); 
            return NULL; 
        }
    }

    return array;
}

void encode_data(Stream stream, uint32_t **array, size_t count) { 
    
    if (array == NULL || count <= 0)
    {
        fprintf(stderr, "Error: Invalid array or count\n");
        exit(EXIT_FAILURE);
    }

    size_t counter = 0;
    // will hold the value of the numeric value of the ascii str 
    uint32_t str_sum_conversion = 0;
    // hold the value of the inner loop variable 
    uint32_t sums[count];

    for (size_t i = 0; i < count; i++)
    {
        char *str = (char *)((char *)stream.packets + counter);
        printf("outside: %s\n", str);
        // increase the counter to get to the right address and retrieve the correct string 
        counter += *(stream.sizes + i);

        for (size_t j = 0; j < *(stream.sizes + i); j++) {
            printf("[%zu][%zu] = %d\n",i,j, (uint32_t)*(str + j)); 
            str_sum_conversion += (uint32_t)*(str + j);
        } 

        printf("uint32_t representation: %u\n", str_sum_conversion);
        printf("-----------------\n");

        *(sums + i) = str_sum_conversion;
        str_sum_conversion = 0;
    }

    // stores the encoded sum value 
    for (size_t i = 0; i < count; i++) {
        uint32_t *p = *(array + i);
        *p = *(sums + i);
    } 
}


void decode_data(Stream stream, uint32_t **array, size_t count) {
    
    if (array == NULL || count <= 0)
    {
        fprintf(stderr, "Error: Invalid array or count\n");
        exit(EXIT_FAILURE);
    }

    uint32_t sum = 0;

    for (size_t i = 0; i < count; i++) {
        uint32_t *p = *(array + i);
        printf("encoded value: %u\n", *p);
        // start the decoding process
        char *s = (char *)((char *)stream.packets + *(stream.offsets + i));
        
        for (size_t j = 0; j < *(stream.sizes + i); j++) {
            uint8_t v = *(s + j);
            sum += v;
        }
        
        assert(*p == sum); 
        printf("decoded value: %s\n", s);
        sum = 0;
        printf("-----------------\n");
    }
}

void free_jagged_array(uint32_t **jagged_array, size_t num_rows) {
    if (jagged_array == NULL) return;

    for (size_t i = 0; i < num_rows; i++) {
        if (*(jagged_array + i) != NULL) free(*(jagged_array + i));
    }

    free(jagged_array);
}
