#include <stdio.h>
#include "util.h"

void draw(Block* blocks) {
    // Initialize cairo surface
    // Iterate thourhg each block and draw it to cairo surface
    //
    // Buffer for sample
    int128_t samp_buf = 0;
    // amplitude will store the two's complement of samp_buf
    int128_t amplitude = 0;

    // Create a mask to select only bits_per_sample from amplitude.
    // Without this amplitude also inverts all the bits before bytes we are
    //      interested in. Giving us amplitude values like 0xFFFFFFFF1234, etc
    //      instead of 0x000000001234
    int128_t pos_bitmask = 0x0;
    pos_bitmask          = ~(~pos_bitmask << (bits_per_sample));

    // This bitmask preserves signedness so 0x8AE9 is interpreted as
    //      0xFF...FF8AE9 (since 0x8AE9 is a negative number) which will be
    //      required to preserve signedness when the data is stored in a int128
    int128_t neg_bitmask = ~pos_bitmask;

    uint8_t MSB = 0;

    for (size_t block_i = 0; block_i < block_count; block_i++) {
        // In block_i'th block
        for (size_t sample_i = 0; sample_i < samples_per_block; sample_i++) {
            // In sample_i'th sample
            samp_buf  = 0;
            amplitude = 0;
            for (size_t byte_i = 0; byte_i < bytes_per_sample; byte_i++) {
                // In byte_i'th byte
                samp_buf = samp_buf
                    | blocks[block_i].sample[sample_i].byte[byte_i]
                        << (8 * (byte_i));
            }
            amplitude = ((~samp_buf) + 1);
            // amplitude >> (bits_per_sample - 8) selects the MSB of amplitude,
            //      if it is >=0x80 the amplitude is negative and must be masked
            //      by ORing with neg_bitmask; else amplitude is positive and
            //      must be masked by ANDing with pos_bitmask
            MSB = (amplitude >> (bits_per_sample - 8));
            if (MSB >= 0x80) {
                amplitude = amplitude | neg_bitmask;
            } else {
                amplitude = amplitude & pos_bitmask;
            }
            // Add `amplitude` to whatever heap object here
        }
        printf("\n");
    }
}
