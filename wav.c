#include "defs.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE* input_file = fopen("sample.wav", "rb");
    if (input_file == NULL) { return 1; }

    MasterChunk* master_chunk = parseFile(input_file);
    if (master_chunk == NULL) { return 2; }
    // Close file since nothing left to be read from it.
    fclose(input_file);

    // Create structures to pack raw PCM data into.

    // Calculate bits and bytes per sample.
    uint16_t bits_per_sample  = master_chunk->fmtChunk.bitsPerSample;
    uint16_t bytes_per_sample = (bits_per_sample / 8);

    // Calculate no. of samples per block = equal to no. of channels.
    uint16_t samples_per_block = master_chunk->fmtChunk.channels;

    // Calculate integral value of dataChunk.chunkSize.
    uint32_t data_size = master_chunk->dataChunk.chunkSize;

    // We can figure out the total no. of blocks since we have the total
    //      size of the data.
    size_t block_count;
    block_count = data_size / (samples_per_block * bytes_per_sample);

    // Calculate bytes per block. blockAlign is equal to bytes per block.
    // uint16_t bytes_per_block = master_chunk->fmtChunk.blockAlign;

    // A struct Sample represents the instantaneous sound data for one channel.
    typedef struct {
        // LSB-...-MSB repr. of instantaneous amplitude.
        BYTE byte[bytes_per_sample];
    } Sample;

    // A struct block represents the instantaneous sound data for all channels.
    typedef struct {
        // 1 sample for each channel.
        Sample sample[samples_per_block];
    } Block;

    // Treat "blocks" as an alias for master_chunk.dataChunk.data.
    Block* blocks = master_chunk->dataChunk.data;

    // By pointing a Block* to data, we can treat master_chunk.dataChunk.data as
    //      an array of Block objects and use syntax like below -
    //
    // blocks[x].sample[y].byte[z]; <-- z'th byte of y'th sample of x'th block
    //
    // Now all data can be read by simply looping block_count times over blocks.
    // See appendix for example.

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
    }

    free(master_chunk->dataChunk.data);
    return 0;
}

/*
Appendix

I. Error Codes -
1 - Could not open file.
2 - Could not parse file.
---
II. Print a well-formatted byte -

printf("%02x", byte);
---
III. Pretty print the raw PCM data.

BYTE byte_buf   = 0;
size_t samp_buf = 0;
for (size_t block_i = 0; block_i < block_count; block_i++) {
    // In block_i'th block
    printf("\nBlock %zu at ", block_i);
    printf("addr. 0x%02x:\n", (bytes_per_block * block_i));
    for (size_t sample_i = 0; sample_i < samples_per_block; sample_i++) {
        // In sample_i'th sample
        samp_buf = 0;
        printf("\tSample %zu: ", sample_i);
        for (size_t byte_i = 0; byte_i < bytes_per_sample; byte_i++) {
            // In byte_i'th byte
            byte_buf = blocks[block_i].sample[sample_i].byte[byte_i];
            samp_buf = samp_buf | byte_buf << (8 * (byte_i));
            // printf("%02x",
            // blocks[block_i].sample[sample_i].byte[byte_i]);
        }
        printf("0x%x", samp_buf);
        printf("\n");
    }
}
---
*/
