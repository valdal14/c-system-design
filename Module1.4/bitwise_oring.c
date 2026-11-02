#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * @brief Packs four distinct fields into a single 16-bit protocol header.
 * Ensure each input is masked to its maximum bit-width before shifting so out-of-range
 * inputs cannot bleed into neighboring fields.
 *
 * @param channel 4-bit channel ID (0-15)
 * @param gain 3-bit amplifier gain (0-7)
 * @param seq_num 8-bit packet sequence number (0-255)
 * @param urgent 1-bit priority flag (0-1)
 * @return uint16_t The packed 16-bit header.
 */
uint16_t pack_header(uint8_t channel, uint8_t gain, uint8_t seq_num, uint8_t urgent);

/**
 * @brief Extracts the 3-bit gain value from a packed 16-bit header.
 *
 * @param header The packed 16-bit header.
 * @return uint8_t The extracted gain value (0-7).
 */
uint8_t extract_gain(uint16_t header);

/**
 * @brief Updates ONLY the 3-bit gain field inside an existing 16-bit header
 * without modifying channel, seq_num, or urgent.
 *
 * @param header The existing packed 16-bit header.
 * @param new_gain The new 3-bit gain value (0-7) to insert.
 * @return uint16_t The updated 16-bit header.
 */
uint16_t update_gain(uint16_t header, uint8_t new_gain);

int main(void)
{
    uint16_t header = pack_header(10, 5, 200, 1);
    printf("header: %d\n", header);
    assert(header == 0xE45A);
    uint8_t gain = extract_gain(header);
    printf("gain: %d\n", gain);
    assert(gain == 0x05);
    header = update_gain(header, 0x07);
    printf("header: %d\n", header);
    assert(header == 0xE47A);
    return 0;
}

uint16_t pack_header(uint8_t channel, uint8_t gain, uint8_t seq_num, uint8_t urgent) {
    return ((channel & 0x0F) << 0  |  (gain & 0x07) << 4  |  (seq_num & 0xFF) << 7  |  (urgent & 0x01) << 15);
}

uint8_t extract_gain(uint16_t header) {
    // Shift and then mask 
    return (header >> 4) & 0x07;
}

uint16_t update_gain(uint16_t header, uint8_t new_gain) {
    // Evict the old value 
    header &= ~(0x07 << 4);
    // Clamp the new gain (max value 7)
    header |= (new_gain & 0x07) << 4;
    return header;
}

