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

    // Create block and sample structures to represent raw PCM data
    // Calculate bits and bytes per sample
    uint16_t bits_per_sample
        = sumNBytesFrom(master_chunk->fmtChunk.bitsPerSample, 2);
    uint16_t bytes_per_sample = (bits_per_sample / 8);

    // Calculate no. of samples per block = equal to no. of channels
    uint16_t samples_per_block
        = sumNBytesFrom(master_chunk->fmtChunk.channels, 2);

    // Calculate integral value of dataChunk.chunkSize
    uint32_t data_size = sumNBytesFrom(master_chunk->dataChunk.chunkSize, 4);

    // We can figure out the total no. of blocks since we have the total
    // size of the data
    size_t block_count;
    block_count = data_size / (samples_per_block * bytes_per_sample);

    // Calculate bytes per block. blockAlign is equal to bytes per block
    uint16_t bytes_per_block
        = sumNBytesFrom(master_chunk->fmtChunk.blockAlign, 2);

    // A struct Sample represents the instantaneous sound data for one
    // channel
    typedef struct {
        // LSB-MSB repr. of instantaenous amplitude
        BYTE byte[bytes_per_sample];
    } Sample;

    // A struct block represents the instantaneous sound data for all
    // channels
    typedef struct {
        // 1 sample for each channel.
        Sample sample[samples_per_block];
    } Block;

    // Treat "blocks" as an alias for master_chunk.dataChunk.data
    Block* blocks = master_chunk->dataChunk.data;

    // By pointing a Block* to data, we can treat master_chunk.dataChunk.data as
    // an array of blocks and use syntax like below -
    //
    // blocks[x].sample[y].byte[z]; <-- z'th byte of y'th sample of x'th block
    //
    // Now all data can be read by simply looping block_count times over blocks.
    // See appendix for example

    free(master_chunk->dataChunk.data);
    return 0;
}

/*
Appendix

I. Error Codes -
1 - not a riff file, not a wav file, or wav file not using pcm encoding
2 - malloc failed
3 - feof or ferror
4 - file couldn't be opened
---
II. Print a well-formatted byte -

printf("%02x", byte);
---
III. Print raw bytes of data with format - 0xBE: 0xEF, where 0xBE is byte
    position starting from pos'th data byte and 0xEF is the byte at pos -

size_t pos = 0;
size_t end_pos = 100;
for (; pos < end_pos; pos) {
  printf("%02x: ", pos);
  printf("%02x", *(riff_chunk.dataChunk.data + pos));
  printf("\n");
}
---
IV. Print the blockbuf in the loop used to read PCM data. Useful for inspecting
    which bytes we are actually reading. This is only for 2 sample 2 byte (i.e.,
    2 channel, 16-bit PCM data) Blocks but the 0/1 values can be swapped out for
    any other block size.

printf("%02x:\t", (blockAlign * block_index));
printf("%02x", blockbuf.sample[0].byte[0]);
printf("%02x", blockbuf.sample[0].byte[1]);
printf("%02x", blockbuf.sample[1].byte[0]);
printf("%02x\n", blockbuf.sample[1].byte[1]);
---
*/
