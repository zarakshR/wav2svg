//
// Created by zaraksh on 10/12/22.
//

#include "util.h"
#include <stdint.h>
#include <stdio.h>

#ifndef WAV2SVG_PARSER_H
#define WAV2SVG_PARSER_H

#define MASTER_CHUNK_HEADER_SIZE 12

// chunkSize never includes the 8 bytes from chunkID and itself
// Everything except chunkIDs are little-endian
// PCM data is signed 2's-complement except for resolutions of 1-8 bits, which
//      are represented as offset binary.

typedef struct _FormatChunk {
    BYTE chunkID[4];        // Should be "fmt ". Note the trailing space.
    uint32_t chunkSize;     // Chunk size.
    BYTE formatCode[2];     // Should be 0x10 0x00 for PCM data.
    uint16_t channels;      // No. of channels = samples per block.
    uint32_t sampleRate;    // Samples per second.
    uint32_t avgByteRate;   // Bytes per second.
    uint16_t blockAlign;    // Bytes per block.
    uint16_t bitsPerSample; // Bits per sample = 8 * Bytes per sample.
} FormatChunk;

typedef struct _DataChunk {
    BYTE chunkID[4];    // Should be "data".
    uint32_t chunkSize; // Chunk size
    void* data;         // Pointer to raw data
} DataChunk;

// Format chunk and data chunk are siblings and children of master chunk
typedef struct _MasterChunk {
    BYTE chunkID[4];    // Should be "RIFF".
    uint32_t chunkSize; // This includes the dataChunk.data
    BYTE format[4];     // Wave ID. Should be "WAVE".
    FormatChunk fmtChunk;
    DataChunk dataChunk;
} MasterChunk;

MasterChunk* parseFile(FILE* input_file);

#endif //WAV2SVG_PARSER_H
