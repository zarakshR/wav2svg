#include "parser.h"
#include "draw.h"
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

    // Send data to draw function
    draw(master_chunk);

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
