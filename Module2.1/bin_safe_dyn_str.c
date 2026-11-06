#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_SIZE 32
#define TYPE_SIZE 32 

typedef struct 
{
    void *buffer;
    size_t current_size;
    size_t max_size;
    size_t required_type_size; 
} SDSDataModel;

typedef struct
{
    uint64_t uuid;
    uint64_t client_id;
    uint64_t client_secret;
    uint64_t certificate_data;
} Data;

/**
 * @brief Allocates a safe SDSDataModel 
 *
 * @param sds SDSDataModel double pointer
 * @param max_alloc_size size_t
 * @param required_type_size size_t
 * @return void
 */
void init(SDSDataModel **sds, size_t max_alloc_size, size_t required_type_size);

/**
 * @brief Allocates data inside the SDSDataModel buffer
 *
 * @param data Data pointer
 * @param sds SDSDataModel pointer
 * return int 0 if failed 1 if success
 */
uint8_t allocate(Data *data, SDSDataModel *sds); 

/**
 * @brief Clean ups the allocated memory
 *
 * @param sds SDSDataModel pointer
 * @return void
 */
void cleanup(SDSDataModel *sds);

int main(void)
{
    SDSDataModel *sds = NULL;
    init(&sds, MAX_SIZE, TYPE_SIZE);
    printf("SDSDataModel allocated at address: %p\n- Buffer is allocated at address: %p\n", (void *)sds, (void *)sds->buffer);
    printf("- Current Buffer Size: %zu\n\n", sds->current_size * TYPE_SIZE);
   
    Data d1;
    d1.uuid = 113456789;
    d1.client_id = 1478;
    d1.client_secret = 0xADC324;
    d1.certificate_data = 0xABEF22;

    Data d2;
    d2.uuid = 123446719;
    d2.client_id = 3478;
    d2.client_secret = 0xADC124;
    d2.certificate_data = 0xABEF22;

    // start data allocation
    uint8_t all_res1 = allocate(&d1, sds);
    
    if (all_res1 == 1)
    {
        Data *d = (Data *)((char *)sds->buffer);
        printf("SDSDataModel uuid: %llu\n", d->uuid);
        printf("Current Buffer Size: %zu\n\n", sds->current_size * TYPE_SIZE);
    }
    
    uint8_t all_res2 = allocate(&d2, sds);

    if (all_res2 == 1)
    {
        Data *d = (Data *)((char *)sds->buffer + ((sds->current_size - 1) * TYPE_SIZE));
        printf("SDSDataModel uuid: %llu\n", d->uuid);
        printf("Current Buffer Size: %zu\n\n", sds->current_size * TYPE_SIZE);
    }
    
    Data d3;
    d3.uuid = 163446719;
    d3.client_id = 3348;
    d3.client_secret = 0xADC167;
    d3.certificate_data = 0xABEF22;

    uint8_t all_res3 = allocate(&d3, sds);

    if (all_res3 == 1)
    {
        Data *d = (Data *)((char *)sds->buffer + ((sds->current_size - 1) * TYPE_SIZE));
        printf("SDSDataModel uuid: %llu\n", d->uuid);
        printf("Current Buffer Size: %zu\n\n", sds->current_size * TYPE_SIZE);
    }

    Data d4;
    d4.uuid = 194546513;
    d4.client_id = 2234;
    d4.client_secret = 0xADC111;
    d4.certificate_data = 0xABEF22;

    uint8_t all_res4 = allocate(&d4, sds);

    if (all_res4 == 1)
    {
        Data *d = (Data *)((char *)sds->buffer + ((sds->current_size - 1) * TYPE_SIZE));
        printf("SDSDataModel uuid: %llu\n", d->uuid);
        printf("Current Buffer Size: %zu\n\n", sds->current_size * TYPE_SIZE);
    }

    Data d5;
    d5.uuid = 202146517;
    d5.client_id = 5678;
    d5.client_secret = 0xADC161;
    d5.certificate_data = 0xABEF22;

    uint8_t all_res5 = allocate(&d5, sds);

    if (all_res5 == 1)
    {
        Data *d = (Data *)((char *)sds->buffer + ((sds->current_size - 1) * TYPE_SIZE));
        printf("SDSDataModel uuid: %llu\n", d->uuid);
        printf("Current Buffer Size: %zu\n\n", sds->current_size * TYPE_SIZE);
    }

    cleanup(sds);

    return 0;
}

void init(SDSDataModel **sds, size_t max_alloc_size, size_t required_type_size) {
    
    if (max_alloc_size <= 0 || max_alloc_size > MAX_SIZE)
    {
        fprintf(stderr, "Invalid size value provided. Must be > 0 and < than %d\n", MAX_SIZE);
        exit(EXIT_FAILURE);
    }

    if (required_type_size != TYPE_SIZE)
    {
        fprintf(stderr, "Unsupported data type. The interface only accept blocks of %d bytes\n", TYPE_SIZE);
        exit(EXIT_FAILURE);
    }

    if (sds == NULL) 
    {
        fprintf(stderr, "Cannot allocate from an empty SDSDataModel\n");
        exit(EXIT_FAILURE);
    }

    SDSDataModel *new_sds = (SDSDataModel *)calloc(1, sizeof(SDSDataModel));

    if (new_sds == NULL)
    {
        fprintf(stderr, "Cannot allocate space for the new SDSDataModel\n");
        exit(EXIT_FAILURE);
    }

    new_sds->buffer = calloc(1, (sizeof(char) * max_alloc_size));

    if (new_sds->buffer == NULL) 
    {
        fprintf(stderr, "Cannot allocate space for the buffer\n");
        exit(EXIT_FAILURE);
    }

    new_sds->current_size = 0;
    new_sds->max_size = max_alloc_size;
    new_sds->required_type_size = required_type_size;

    *sds = new_sds;
}

uint8_t allocate(Data *data, SDSDataModel *sds) {
    
    if (data == NULL || sds == NULL) return 0;

    if (((sds->current_size + 1) * TYPE_SIZE) > sds->max_size)
    {
        /**
         * Attempt to realloc the current buffer. If the operation
         * fails the buffer will be cleaned and all data will be 
         * deallocated.
         */
        printf("---------------------------------------------------------------------------------\n");
        printf("Max capacity reached, reallocating the buffer\n");
        // calculate the new size 
        size_t new_size = (sds->max_size * sds->current_size) * 2;
        void *ptr = realloc(sds->buffer, (sizeof(char) * new_size));

        if (ptr == NULL) {
            fprintf(stderr, "Reallocating the buffer failed\n");
            cleanup(sds);
            return 0;

        } else {
            // if realloc is successfull set the new max_size 
            sds->max_size = new_size;
            // assign the prt to the buffer to make the reallocation effective
            sds->buffer = ptr;
            printf("Buffer reallocation completed at address: %p -  MAX_SIZE = %zu\n", (void *)ptr, sds->max_size); 
            printf("---------------------------------------------------------------------------------\n\n");
        }
    }
        
    Data *d = (Data *)((char *)sds->buffer + (sds->current_size * TYPE_SIZE));

    if (d == NULL)  { 
        fprintf(stderr, "Could not get memory from the buffer\n");
        return 0;
    }
 
    d->uuid = data->uuid;
    d->client_id = data->client_id;
    d->client_id = data->client_secret;
    d->certificate_data = data->certificate_data;
    // increase the size of the buffer
    sds->current_size += 1;
    printf("Data stored at address: %p\n", (void *)d);
    return 1; 
}

void cleanup(SDSDataModel *sds) {
    if (sds == NULL) return;
    free(sds->buffer);
    free(sds);
}
