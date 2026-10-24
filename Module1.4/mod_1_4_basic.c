#include <assert.h>
#include <stdio.h>
#include <stdlib.h> 
#include <stdint.h>

#define BIT(x) (1 << (x))
#define HEX(x) printf("Hex value: 0x%02X\n", x)
#define DEC(x) printf("Dec value: %d\n", x)

#define FLAG_ACTIVE_BIT     0 
#define FLAG_COMPRESSED_BIT 1 
#define FLAG_ENCRYPTED_BIT  2 
#define FLAG_ERROR_BIT      3 

/**
 * @brief Sets a specific flag to 1 without altering other bits.
 * @param register_val Out-Parameter pointer to the status register.
 * @param flag         The bitmask of the flag to set.
 */
void set_flag(uint8_t *register_val, uint8_t flag);

/**
 * @brief Clears a specific flag to 0 without altering other bits.
 * @param register_val Out-Parameter pointer to the status register.
 * @param flag         The bitmask of the flag to clear.
 */
void clear_flag(uint8_t *register_val, uint8_t flag);

/**
 * @brief Toggles a specific flag (0 becomes 1, 1 becomes 0) without altering others.
 * @param register_val Out-Parameter pointer to the status register.
 * @param flag         The bitmask of the flag to toggle.
 */
void toggle_flag(uint8_t *register_val, uint8_t flag);

/**
 * @brief Checks if a specific flag is currently set to 1.
 * @param register_val The current value of the status register.
 * @param flag         The bitmask of the flag to check.
 * @return 1 if the flag is set, 0 if it is not.
 */
uint8_t check_flag(uint8_t register_val, uint8_t flag);

/**
 * @brief Prints out the current Hex and Decimal value of the 
 * given argument
 *
 * @param register_val uint8_t
 * @return void
 */
void print_status(uint8_t register_val);

/// MAIN -----------------------------------------------------------------------------

int main(void) 
{
    uint8_t status = 0x00;
    
    set_flag(&status, FLAG_ACTIVE_BIT);
    set_flag(&status, FLAG_ENCRYPTED_BIT);
    print_status(status);
    assert(status == 0x05);

    toggle_flag(&status, FLAG_ENCRYPTED_BIT);
    assert(status == 0x01);
    
    toggle_flag(&status, FLAG_ENCRYPTED_BIT);
    print_status(status);
    assert(status == 0x05);

    clear_flag(&status, FLAG_ENCRYPTED_BIT);
    print_status(status);
    assert(status == 0x01);
    
    clear_flag(&status, FLAG_ENCRYPTED_BIT);
    print_status(status);
    assert(status == 0x01);


    toggle_flag(&status, FLAG_ACTIVE_BIT);
    assert(status == 0x00);
    print_status(status);

    return 0;
}

/// IMPLEMENTATION -------------------------------------------------------------------
                             
void set_flag(uint8_t *register_val, uint8_t flag) {
    *register_val |= BIT(flag);
}

void clear_flag(uint8_t *register_val, uint8_t flag) { 
    *register_val &= ~BIT(flag);
}

void toggle_flag(uint8_t *register_val, uint8_t flag) {
    *register_val ^= BIT(flag);
}

uint8_t check_flag(uint8_t register_val, uint8_t flag) {
    return (register_val & BIT(flag)) != 0; 
}

void print_status(uint8_t register_val) {
    DEC(register_val);
    HEX(register_val);
    printf("----------------\n");
}
