#include <stdint.h>
#include <stdio.h>

#define BYTE uint8_t
#define MASTER_CHUNK_HEADER_SIZE 12

// chunkSize never includes the 8 bytes from chunkID and itself
// Everything except chunkIDs are little-endian
// PCM data is signed 2's-complement except for resolutions of 1-8 bits, which
//      are represented as offset binary.

typedef struct _FormatChunk {
    BYTE chunkID[4];       // Should be "fmt ". Note the trailing space.
    BYTE chunkSize[4];     // Chunk size.
    BYTE formatCode[2];    // Should be 0x10 0x00 for PCM data.
    BYTE channels[2];      // No. of channels = samples per block.
    BYTE sampleRate[4];    // Samples per second.
    BYTE avgByteRate[4];   // Bytes per second.
    BYTE blockAlign[2];    // Bytes per block.
    BYTE bitsPerSample[2]; // Bits per sample = 8 * Bytes per sample.
} FormatChunk;

typedef struct _DataChunk {
    BYTE chunkID[4];   // Should be "data".
    BYTE chunkSize[4]; // Chunk size
    void* data;        // Pointer to raw data
} DataChunk;

// Format chunk and data chunk are siblings and children of master chunk
typedef struct _MasterChunk {
    BYTE chunkID[4];   // Should be "RIFF".
    BYTE chunkSize[4]; // This includes the dataChunk.data
    BYTE format[4];    // Wave ID. Should be "WAVE".
    FormatChunk fmtChunk;
    DataChunk dataChunk;
} MasterChunk;

uint64_t sumNBytesFrom(const BYTE* seriesOfBytes, uint32_t N);
MasterChunk* parseFile(FILE* input_file);
