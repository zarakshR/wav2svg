#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE uint8_t
#define BYTE_SIZE sizeof(BYTE)
#define RIFF_CHUNK_HEADER_SIZE 12

// chunkSize never includes the 8 bytes from chunkID and itself
// Everything except chunkIDs are little-endian
// Block and  sample frame are synonymous
// PCM data is signed 2's-complement except for resolutions of 1-8 bits, which are
// represented as offset binary.

typedef struct {    // 24 bytes in length.
  BYTE chunkID[4];  // Should be "fmt ". Note the trailing space.
  BYTE chunkSize[4];
  BYTE formatCode[2];     // Should be 0x10 0x00 for PCM data
  BYTE channels[2];       // No. of channels -- Also samples per block
  BYTE sampleRate[4];     // Samples per second.
  BYTE avgByteRate[4];    // Bytes per second
  BYTE blockAlign[2];     // Bytes per block
  BYTE bitsPerSample[2];  // Bits per sample = 8 * Bytes per sample. This field
                          // is required for LPCM encoded data.
} FormatChunk;

typedef struct {
  BYTE chunkID[4];  // Should be "data"
  BYTE chunkSize[4];
  BYTE* data;  // use chunkSize to malloc this
} DataChunk;

// Format chunk and data chunk are siblings and children of RIFF chunk
typedef struct {      // 12 bytes in length.
  BYTE chunkID[4];    // Chunk ID. Should be "RIFF".
  BYTE chunkSize[4];  // This includes the dataChunk.data
  BYTE format[4];     // Wave ID. Should be "WAVE".
  FormatChunk fmtChunk;
  DataChunk dataChunk;
} RiffChunk;

int main() {
  size_t _N;  // Variable used in loops to sum up strings of bytes. See Appendix
  int8_t _i;  // Index variable for loop. Should not be unsigned

  FILE* input_file = fopen("sample.wav", "rb");
  if (input_file == NULL) {
    return 4;
  }

  RiffChunk riff_chunk;

  // Read RIFF chunk header
  fread(&riff_chunk, RIFF_CHUNK_HEADER_SIZE, 1, input_file);
  // Read format chunk
  fread(&riff_chunk.fmtChunk, sizeof(riff_chunk.fmtChunk), 1, input_file);

  // This is the format code for linear PCM encoding
  BYTE fmt_pcm_sig[2] = {0x10, 0x00};

  // Make sure that file is a WAVE RIFF file with linear PCM encoding.
  BYTE signal = 0;

  // Check if RIFF
  signal += (memcmp(riff_chunk.chunkID, "RIFF", BYTE_SIZE * 4));
  // Check if WAVE
  signal += (memcmp(riff_chunk.format, "WAVE", BYTE_SIZE * 4));
  // Check if using linear PCM encoding
  signal += (memcmp(riff_chunk.fmtChunk.formatCode, fmt_pcm_sig,
                    sizeof(fmt_pcm_sig)));

  if (signal != 0) {
    return 1;
  }

  // Read file until "data" bytes are found
  // We have to read manually in groups of 4 bytes starting from every byte
  // because the chunkID bytes may not be 4-aligned due to undefined chunks not
  // being a multiple of 4 in length.
  BYTE buffer[4];
  for (;;) {
    fread(&buffer, 4, 1, input_file);
    if ((memcmp(&buffer, "data", 4)) == 0) {  // "data" found
      break;
    }
    fseek(input_file, -3, SEEK_CUR);  // Go back 3 bytes
  }
  // Write "data" into chunkID
  strcpy(riff_chunk.dataChunk.chunkID, buffer);

  // Read dataChunk.chunkSize. This is the total no. of PCM data bytes.
  fread(&riff_chunk.dataChunk.chunkSize, 4, 1, input_file);

  // Convert dataChunk.chunkSize into an integral format by summing the 4 bytes
  _N = 4;
  uint32_t data_size = 0;
  for (_i = (_N - 1); _i >= 0; _i--) {
    data_size = data_size << 8 | riff_chunk.dataChunk.chunkSize[_i];
  }

  // Allocate data_size bytes of data for dataChunk.data
  riff_chunk.dataChunk.data = (BYTE*)malloc(data_size);
  if (riff_chunk.dataChunk.data == NULL) {
    return 2;
  }

  // Read data_size bytes of data into dataChunk.data
  fread(riff_chunk.dataChunk.data, data_size, 1, input_file);
  if (feof(input_file) || ferror(input_file)) {
    return 3;
  }
  fclose(input_file);  // Close file since nothing left to be read from it.

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
III. Sum a series of N bytes at seriesOfBytes -

_N = ?;
?? ?sum? = 0;
for (_i = (_N-1); _i >= 0; _i--) {
  ?sum? = ?sum? << 8 | ?seriesOfBytes?[_i];
}
---
IV. Print raw bytes of data with format - 0xBE: 0xEF, where 0xBE is byte
    position starting from pos'th data byte and 0xEF is the byte at pos -

size_t pos = 0;
size_t end_pos = 100;
for (; pos < end_pos; pos) {
  printf("%02x: ", pos);
  printf("%02x", *(riff_chunk.dataChunk.data + pos));
  printf("\n");
}
*/
