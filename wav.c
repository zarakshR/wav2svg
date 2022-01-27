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

    // We can figure out the total no. of blocks since we have the total
    // size of the data
    size_t block_count;
    block_count = data_size / (samples_per_block * bytes_per_sample);

    // Calculate blockAlign
    uint16_t blockAlign = sumNBytesFrom(riff_chunk.fmtChunk.blockAlign, 2);
    // blockAlign is the same thing as bytes per block
    uint16_t bytes_per_block = blockAlign;

    // These will be needed to read the data
    BYTE* data_ptr = riff_chunk.dataChunk.data;
    BYTE byte;
    Block blockbuf;
    uint32_t block_index;
    size_t block_offset;
    uint16_t sample_index;
    size_t sample_offset;
    uint16_t byte_index;
    size_t byte_offset;

    // Loop over each block
    for (block_index = 0; block_index < block_count; block_index++) {
        block_offset = block_index * bytes_per_block;

        // Loop over each sample in the block
        for (sample_index = 0; sample_index < samples_per_block;
             sample_index++) {
            sample_offset = sample_index * bytes_per_sample;

            // Loop over each byte in the sample
            for (byte_index = 0; byte_index < bytes_per_sample; byte_index++) {
                byte_offset = byte_index; // Trivial, included for clarity

                byte = *(data_ptr + block_offset + sample_offset + byte_offset);
                blockbuf.sample[sample_index].byte[byte_index] = byte;
            }
        }
        // At this point we have a structured representation of a block of data.
    }
    free(riff_chunk.dataChunk.data);
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
