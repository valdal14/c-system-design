#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define REG_POWER     0x01
#define REG_TX_READY  0x02
#define REG_RX_READY  0x04
#define REG_CLK_SYNC  0x08
#define REG_LOW_BATT  0x10
#define REG_OVERHEAT  0x20 
#define REG_CRC_ERR   0x40 
#define REG_HEARTBEAT 0x80 

/**
 * @brief Helper macro used to detect whether a 
 * flag is enable (1) or not (0). Perfomrs a bitwise 
 * operation on given value (val) against an HEX mask (hm)
 * 
 * @param val uint8_t value
 * @param hm HEX mask
 * @return 1 if the mask in on or 0 if it is off
 */
#define IS_FLAG_ENABLE(val,hm) (((val) & (hm)) != 0)

/**
 * @brief Helper macro used to clear or set the value
 * of the given value (val) to 0 given a HEX mash (hm)
 *
 * @param val uint8_t value
 * @param hm HEX mask
 * @return new uint8_t value 
 */
#define CLEAN_FLAG(val,hm) ((val) &= ~(hm))

/**
 * @brief Helper macro used to toggle the bit of 
 * a given value (val) using the HEX mask (hm)
 *
 * @param val uint8_t value
 * @param hm HEX mask
 * @return new uint8_t value 
 */
#define TOGGLE_FLAG(val, hm) ((val) ^= (hm))

/**
 * @brief Helper macro used to force the flag of the
 * given HEX mask to be set to 0 based on the value (val)
 * provided
 *
 * @param val uint8_t value
 * @param hm HEX mask
 * @return new uint8_t value 
 */
#define ENABLE_FLAG(val, hm) ((val) |= (hm))

/**
 * @brief Checks if the gateway is ready to transmit.
 * The gateway can only transmit if REG_POWER, REG_TX_READY, and REG_CLK_SYNC
 * are ALL set to 1 simultaneously, AND neither REG_OVERHEAT nor REG_CRC_ERR is set.
 *
 * @param reg The current 8-bit register value (passed by value, read-only).
 * @return uint8_t Returns 1 if ready to transmit, 0 otherwise.
 */
uint8_t is_gateway_ready(uint8_t reg);

/**
 * @brief Clears both fault flags (REG_OVERHEAT and REG_CRC_ERR) in a single
 * bitwise operation while leaving all other 6 bits untouched.
 *
 * @param reg The current 8-bit register value.
 * @return uint8_t The updated 8-bit register value.
 */
uint8_t clear_faults(uint8_t reg);

/**
 * @brief Flips the REG_HEARTBEAT bit (0->1 or 1->0) and forces REG_POWER to 1
 * while preserving the state of bits 1 through 6.
 *
 * @param reg The current 8-bit register value.
 * @return uint8_t The updated 8-bit register value.
 */
uint8_t pulse_heartbeat(uint8_t reg);

int main(void)
{
    uint8_t reg = 0x0B;
    uint8_t reg_status;
    reg_status = is_gateway_ready(reg);
    assert(reg_status == 1);

    reg = 0x2B;
    reg_status = is_gateway_ready(reg);
    assert(reg_status == 0);

    reg = 0x03;
    reg_status = is_gateway_ready(reg);
    assert(reg_status == 0);

    reg = 0xFF;
    uint8_t value = clear_faults(reg);
    assert(value == 0x9F && value == 159);
    
    reg = 0x00;
    value = pulse_heartbeat(reg);
    assert(value == 0x81 && value == 129);
    value = pulse_heartbeat(value);
    assert(value == 0x01 && value == 1);

    return 0;
}

uint8_t is_gateway_ready(uint8_t reg) {
    printf("reg: %d\n", reg);

    // Evaluate operational flags
    uint8_t power = IS_FLAG_ENABLE(reg, REG_POWER);
    uint8_t tx_ready = IS_FLAG_ENABLE(reg, REG_TX_READY);
    uint8_t clk_sync = IS_FLAG_ENABLE(reg, REG_CLK_SYNC);

    // Evaluate fault flags
    uint8_t overheat = IS_FLAG_ENABLE(reg, REG_OVERHEAT);
    uint8_t crc_err = IS_FLAG_ENABLE(reg, REG_CRC_ERR);

    printf("power: %d, tx: %d, clk: %d\n", power, tx_ready, clk_sync);
    printf("overheat: %d, crc_err: %d\n", overheat, crc_err);

    // Ensure operational flags are ON and fault flags are OFF
    uint8_t state = (power && tx_ready && clk_sync && !overheat && !crc_err) ? 1 : 0;

    printf("state: %d\n", state);
    printf("--------------------------\n");
    return state;
}

uint8_t clear_faults(uint8_t reg) {
    CLEAN_FLAG(reg, REG_OVERHEAT);
    CLEAN_FLAG(reg, REG_CRC_ERR);
    return reg;  
}

uint8_t pulse_heartbeat(uint8_t reg) {
    TOGGLE_FLAG(reg, REG_HEARTBEAT);
    ENABLE_FLAG(reg, REG_POWER);
    return reg;
}
