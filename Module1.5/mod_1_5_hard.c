#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Forward declaration so the function pointer can reference the struct itself
struct IoT_Sensor; 

/**
 * @brief The function pointer signature for sensor processing.
 * Note the 'self' pointer. This is exactly how Python's 'self' or Java's 'this' works under the hood!
 */
typedef void (*ProcessFunc)(struct IoT_Sensor *self, uint16_t raw_data);

/**
 * @brief The Object struct containing both State (id) and Behavior (process).
 */
typedef struct IoT_Sensor {
    uint32_t id;
    ProcessFunc process;
} IoT_Sensor;

/**
 * @brief Concrete implementation for Temperature processing.
 */
void process_temperature(IoT_Sensor *self, uint16_t raw_data);

/**
 * @brief Concrete implementation for Humidity processing.
 */
void process_humidity(IoT_Sensor *self, uint16_t raw_data);

int main(void) 
{
    
    IoT_Sensor temp_sensor;
    temp_sensor.id = 101;
    temp_sensor.process = process_temperature;

    IoT_Sensor hum_sensor;
    hum_sensor.id = 202;
    hum_sensor.process = process_humidity;
    
    // dispatch table 
    IoT_Sensor *network[] = { &temp_sensor, &hum_sensor };
    
    for (size_t i = 0; i < 2; i++)
    {
        network[i]->process(network[i], 45);
    }

    return 0;
}

void process_humidity(IoT_Sensor *self, uint16_t raw_data) {
    printf("Humidity Sensor [%u]   : %u C\n", self->id, raw_data);
}

void process_temperature(IoT_Sensor *self, uint16_t raw_data) {
    printf("Temperature Sensor [%u]: %u C\n", self->id, raw_data);
}
