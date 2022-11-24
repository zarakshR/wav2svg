#include <stdio.h>
#include "draw.h"

void draw(MasterChunk* master_chunk) {
    // Populate metadata about file
    Meta meta;

    // Calculate bits and bytes per sample.
    meta.bits_per_sample  = master_chunk->fmtChunk.bitsPerSample;
    meta.bytes_per_sample = (meta.bits_per_sample / 8);

    // Calculate bytes per block. blockAlign is equal to bytes per block.
    meta.bytes_per_block = master_chunk->fmtChunk.blockAlign;

    // Calculate no. of samples per block = equal to no. of channels.
    meta.samples_per_block = master_chunk->fmtChunk.channels;

    // Calculate integral value of dataChunk.chunkSize.
    meta.data_size = master_chunk->dataChunk.chunkSize;

    // We can figure out the total no. of blocks since we have the total
    //      size of the data.
    meta.block_count = meta.data_size / (meta.samples_per_block * meta.bytes_per_sample);

    // By pointing a Block* to data, we can treat master_chunk.dataChunk.data as
    //      an array of Block objects and use syntax like below -
    //
    // blocks[x].sample[y].byte[z]; <-- z'th byte of y'th sample of x'th block
    //
    // Now all data can be read by simply looping block_count times over blocks.
    // See appendix for example.

    // A struct Sample represents the instantaneous sound data for one channel.
    typedef struct {
        // LSB-...-MSB repr. of instantaneous amplitude.
        BYTE byte[meta.bytes_per_sample];
    } Sample;

    // A struct block represents the instantaneous sound data for all channels.
    typedef struct {
        // 1 sample for each channel.
        Sample sample[meta.samples_per_block];
    } Block;

    // Treat "blocks" as an alias for master_chunk.dataChunk.data.
    Block* blocks = master_chunk->dataChunk.data;

    // Buffer for sample
    int128_t samp_buf = 0;
    // amplitude will store the two's complement of samp_buf
    int128_t amplitude = 0;

    // Create a mask to select only bits_per_sample from amplitude.
    // Without this amplitude also inverts all the bits before bytes we are
    //      interested in. Giving us amplitude values like 0xFFFFFFFF1234, etc
    //      instead of 0x000000001234
    int128_t pos_bitmask = 0x0;
    pos_bitmask          = ~(~pos_bitmask << (meta.bits_per_sample));

    // This bitmask preserves signedness so 0x8AE9 is interpreted as
    //      0xFF...FF8AE9 (since 0x8AE9 is a negative number) which will be
    //      required to preserve signedness when the data is stored in a int128
    int128_t neg_bitmask = ~pos_bitmask;

    uint8_t MSB = 0;

    // TODO: un-hardcode these
    // TODO: figure out a heuristic for x and y resolutions
    double x_resolution = 0.01;
    double y_resolution = 0.005;
    int sample_resolution = 15;

    double width = meta.block_count * x_resolution;

    // ((1<<meta.bits_per_sample)-1) is the maximum possible amplitude for a
    // channel. Multiplied by twice the no. of channels (samples per block)
    // since each channel can hold +ve and -ve values upto 2^bits_per_sample.
    double height = ((meta.samples_per_block * 2) * (((1<<meta.bits_per_sample) - 1) * y_resolution));

    // Initialize cairo surface to draw on
    cairo_surface_t * surface =
        cairo_svg_surface_create("out.svg", width, height);

    // Array of cairo stroke objects representing each channel
    cairo_t * strokes[meta.samples_per_block];
    for (int i = 0; i < meta.samples_per_block; i++) {
        // Create each stroke object
        strokes[i] = cairo_create(surface);
        // Make sure lines are at correct starting point
        cairo_move_to(strokes[i], 0.0, ((i+1) * (height/(meta.samples_per_block+1))));
        // Height is divided into "bands" equal in number to the number of
        // channels. The (i+1) is so that each channel is assigned a
        // corresponding "band"
        // 2 channels => height/3 gives us 0.33, 0.66 as starting points
        // 3 channels => height/4 gives us 0.25, 0.50, 0.75 as starting points
        // 4 channels => height/5 gives us 0.20, 0.40, 0.60, 0.80 as starting points
        // etc.
    }

    // Iterate through each block and draw it to cairo surface
    for (size_t block_i = 0; block_i < meta.block_count; block_i += sample_resolution) {
        // In block_i'th block

        for (size_t sample_i = 0; sample_i < meta.samples_per_block; sample_i++) {
            // In sample_i'th sample

            samp_buf  = 0;
            amplitude = 0;

            for (size_t byte_i = 0; byte_i < meta.bytes_per_sample; byte_i++) {
                // In byte_i'th byte
                // TODO: Performance bottleneck
                //      Maybe try loading samp_buf all in one go?
                samp_buf = samp_buf
                    | blocks[block_i].sample[sample_i].byte[byte_i]
                        << (8 * (byte_i));
            }
            amplitude = ((~samp_buf) + 1); // TODO: I don't remember what this is for

            // 8-bit wav data does not use two's complement. skip below section
            // if file is 8-bit wav.
            if (meta.bits_per_sample <= 8) { goto skip_twos_complement; }

            // amplitude >> (bits_per_sample - 8) selects the MSB of amplitude,
            //      if it is >=0x80 the amplitude is negative and must be masked
            //      by ORing with neg_bitmask; else amplitude is positive and
            //      must be masked by ANDing with pos_bitmask
            MSB = (amplitude >> (meta.bits_per_sample - 8));
            if (MSB >= 0x80) {
                amplitude = amplitude | neg_bitmask;
            } else {
                amplitude = amplitude & pos_bitmask;
            }
            skip_twos_complement:

            // Ayy yo what the fuck is this shit dawg! Refactor!
            cairo_line_to(
                                strokes[sample_i],
                                (block_i * x_resolution),
                                (
                                    // same logic used to divide height up into
                                    // "bands" is used to figure out zero on the
                                    // y-axis
                                    ((sample_i + 1) * (height/(meta.samples_per_block+1)))
                                    +
                                    // add amplitude as offset
                                    (amplitude * y_resolution))
                            );
        }
    }

    // Apply each stroke
    for (int i = 0; i < meta.samples_per_block; i++) {
        cairo_stroke(strokes[i]);
        cairo_close_path(strokes[i]);
    }

    // This is required or nothing gets written to file
    cairo_surface_destroy(surface);

    // Destroy each stroke object
    for (int i = 0; i < meta.samples_per_block; i++) {
        cairo_destroy(strokes[i]);
    }
}
