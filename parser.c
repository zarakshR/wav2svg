#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Takes a pointer to a .wav file and returns a structured representation of the
//      data in it
MasterChunk* parseFile(FILE* input_file)
{
    // This will represent
    static MasterChunk master_chunk;

    // Read RIFF header
    fread(&master_chunk, MASTER_CHUNK_HEADER_SIZE, 1, input_file);
    // Read format chunk
    fread(&master_chunk.fmtChunk, sizeof(master_chunk.fmtChunk), 1, input_file);

    // This is the format code for linear PCM encoding
    BYTE fmt_pcm_sig[2] = { 0x10, 0x00 };

    // Make sure that file is a WAVE RIFF file with linear PCM encoding.
    BYTE signal = 0;

    // Check if RIFF
    signal += (memcmp(master_chunk.chunkID, "RIFF", sizeof(BYTE) * 4));
    // Check if WAVE
    signal += (memcmp(master_chunk.format, "WAVE", sizeof(BYTE) * 4));
    // Check if using linear PCM encoding
    signal += (memcmp(master_chunk.fmtChunk.formatCode, fmt_pcm_sig,
                      sizeof(fmt_pcm_sig)));

    if (signal != 0) { return NULL; }

    // Read file until "data" bytes are found
    // We have to read manually in groups of 4 bytes starting from every
    // byte because the chunkID bytes may not be 4-aligned due to undefined
    // chunks not being a multiple of 4 in length.
    BYTE buf[4];
    for (;;) {
        fread(&buf, 4, 1, input_file);
        if ((memcmp(&buf, "data", 4)) == 0) { break; } // "data" found
        fseek(input_file, -3, SEEK_CUR);               // Go back 3 bytes
    }
    // Write "data" into chunkID
    strcpy(master_chunk.dataChunk.chunkID, buf);

    // Read dataChunk.chunkSize. This is the total no. of PCM data bytes.
    fread(&master_chunk.dataChunk.chunkSize, 4, 1, input_file);

    // Convert dataChunk.chunkSize into an integral format by summing the 4
    // bytes
    uint32_t data_size = sumNBytesFrom(master_chunk.dataChunk.chunkSize, 4);

    // Allocate data_size bytes of data for dataChunk.data
    master_chunk.dataChunk.data = (BYTE*)malloc(data_size);
    if (master_chunk.dataChunk.data == NULL) { return NULL; }

    // Read data_size bytes of data into dataChunk.data
    fread(master_chunk.dataChunk.data, data_size, 1, input_file);
    if (feof(input_file) || ferror(input_file)) { return NULL; }

    return &master_chunk;
}
