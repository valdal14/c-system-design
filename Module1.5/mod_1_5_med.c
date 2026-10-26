#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
/**
 * @brief Pipeline lifecycle functions.
 */
void init_pipeline(void);
void process_data(void);
void shutdown_pipeline(void);

/**
 * @brief Type definition for a pipeline function pointer that takes void and returns void.
 */
typedef void (*PipelineAction)(void);

int main(void) 
{
    PipelineAction dispatch_table[3] = {init_pipeline, process_data, shutdown_pipeline};
    // Simulate an incoming stream of commands
    uint8_t commands[] = {0, 1, 1, 2};
    
    for (size_t i = 0; i < (sizeof(commands) / sizeof(uint8_t)); i++) {
        if (*(commands + i) < 3)
        {
            PipelineAction f = *(PipelineAction *)(dispatch_table + *(commands + i));
            f();
        }
    }

    return 0;
}

void init_pipeline(void) {
    printf("init pipeline\n");
    sleep(1);
}

void process_data(void) {
    printf("pipeline data processing\n");
    sleep(1);
}

void shutdown_pipeline(void) {
    printf("shutdown pipeline\n");
    sleep(1);
}
