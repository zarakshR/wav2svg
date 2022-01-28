#include "defs.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    size_t _i; // Index variable for loops.

    FILE* input_file = fopen("sample.wav", "rb");
    if (input_file == NULL) { return 1; }

    MasterChunk* master_chunk = parseFile(input_file);
    if (master_chunk == NULL) { return 2; }
    // Close file since nothing left to be read from it.
    fclose(input_file);

    // Create structures to pack raw PCM data into.

    // Calculate bits and bytes per sample.
    uint16_t bits_per_sample
        = sumNBytesFrom(master_chunk->fmtChunk.bitsPerSample, 2);
    uint16_t bytes_per_sample = (bits_per_sample / 8);

    // Calculate no. of samples per block = equal to no. of channels.
    uint16_t samples_per_block
        = sumNBytesFrom(master_chunk->fmtChunk.channels, 2);

    // Calculate integral value of dataChunk.chunkSize.
    uint32_t data_size = sumNBytesFrom(master_chunk->dataChunk.chunkSize, 4);

    // We can figure out the total no. of blocks since we have the total
    //      size of the data.
    size_t block_count;
    block_count = data_size / (samples_per_block * bytes_per_sample);

    // Calculate bytes per block. blockAlign is equal to bytes per block.
    uint16_t bytes_per_block
        = sumNBytesFrom(master_chunk->fmtChunk.blockAlign, 2);

    // A struct Sample represents the instantaneous sound data for one channel.
    typedef struct {
        // LSB-...-MSB repr. of instantaenous amplitude.
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
III. Loop through and print each byte. This can be copy pasted when/if we need
to perform specific operations on the data.

for (size_t block_index = 0; block_index < block_count; block_index++) {
    printf("\n%02x:\t", (bytes_per_block * block_index));
    for (size_t sample_index = 0; sample_index < samples_per_block;
            sample_index++) {
        for (size_t byte_index = 0; byte_index < bytes_per_sample;
                byte_index++) {
            printf(
                "%02x",
                blocks[block_index].sample[sample_index].byte[byte_index]);
        }
    }
}
---
*/
