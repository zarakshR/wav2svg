#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE uint8_t
#define BYTE_SIZE sizeof(BYTE)

// Evrything expect chunk IDs are little-endian

struct riffChunk {    // 12 bytes in length.
  BYTE chunkID[4];    // Chunk ID. Should be "RIFF".
  BYTE chunkSize[4];  // Chunk size.
  BYTE format[4];     // Wave ID. Should be "WAVE".
};

struct fmtChunk {   // 24 bytes in length.
  BYTE chunkID[4];  // Should be "fmt ". Note the trailing space.
  BYTE chunkSize[4];
  BYTE formatCode[2];  // Should be 0x10 0x00
  BYTE channels[2];
  BYTE sampleRate[4];
  BYTE bytesPerSec[4];
  BYTE blockAlign[2];
  BYTE bitsPerSample[2];
};

struct dataChunk {};

int main() {
  FILE* input_file = fopen("sample.wav", "rb");

  struct riffChunk riff_chunk;
  struct fmtChunk fmt_chunk;
  fread(&riff_chunk, sizeof(riff_chunk), 1, input_file);
  fread(&fmt_chunk, sizeof(fmt_chunk), 1, input_file);

  BYTE signal = 0;

  BYTE fmt_formatCode_sig[2] = {0x10, 0x00};
  signal += (memcmp(riff_chunk.chunkID, "RIFF", BYTE_SIZE * 4));
  signal += (memcmp(riff_chunk.format, "WAVE", BYTE_SIZE * 4));
  signal += (memcmp(fmt_chunk.chunkID, "fmt ", sizeof("fmt")));
  signal += (memcmp(fmt_chunk.formatCode, fmt_formatCode_sig,
                    sizeof(fmt_formatCode_sig)));

  if (signal != 0) {
    return 1;
  }
  // File format checking done.

  fclose(input_file);
  return 0;
}

/*
Error Codes -
1 - not a riff file, not a wav file, or wav file not using pcm encoding
*/

// printf("%02x", byte);
// printf("%d", size);

// // Find chunkSize. _i goes from 3 to 0 because chunkSize is little endian
// uint32_t size = 0;  // Since chunkSize is 4 bytes it can't be > 2^32
// for (int8_t _i = 3; _i >= 0; _i--) {
//   size = size << 8 | riff_chunk.chunkSize[_i];
// }
