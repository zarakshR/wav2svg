#ifndef WAV2SVG_DRAW_H
#define WAV2SVG_DRAW_H

#include "parser.h"
#include <cairo/cairo.h>
#include <cairo/cairo-svg.h>

void draw(MasterChunk* master_chunk);

// A struct meta represents metadata for PCM data that needs to be
// interpreted by the drawing function.
typedef struct _Meta {
    uint16_t bits_per_sample;
    uint16_t bytes_per_sample;
    uint16_t bytes_per_block;
    uint16_t samples_per_block;
    uint32_t data_size;
    size_t block_count;
} Meta;

#endif //WAV2SVG_DRAW_H
