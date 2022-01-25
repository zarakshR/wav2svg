#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE uint8_t
#define BYTE_SIZE sizeof(BYTE)

// Everything except chunk IDs are little-endian

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

  // This is the format code for PCM encoding
  BYTE fmt_pcm_sig[2] = {0x10, 0x00};

  // Check if RIFF
  signal += (memcmp(riff_chunk.chunkID, "RIFF", BYTE_SIZE * 4));
  // Check if WAVE
  signal += (memcmp(riff_chunk.format, "WAVE", BYTE_SIZE * 4));
  // Check if using PCM encoding
  signal += (memcmp(fmt_chunk.formatCode, fmt_pcm_sig, sizeof(fmt_pcm_sig)));

  if (signal != 0) {
    return 1;
  }
  // File format checking done.

  fclose(input_file);
  return 0;
}

/*
Appendix

I. Error Codes -
1 - not a riff file, not a wav file, or wav file not using pcm encoding
2 - malloc failed
3 - feof or ferror

II. Print a well-formatted byte
printf("%02x", byte);

III. Sum a series of N bytes at seriesOfBytes
size_t N = 3;
uint32_t size = 0;
for (int8_t _i = N; _i >= 0; _i--) {
  size = size << 8 | seriesOfBytes[_i];
}

IV. Print raw bytes of data with format - 0xBE: 0xEF, where 0xBE is byte
    position starting from pos'th data byte and 0xEF is the byte at pos

size_t pos = 0;
size_t end_pos = 100;
for (; pos < end_pos; pos) {
  printf("%02x: ", pos);
  printf("%02x", *(data_chunk.data + pos));
  printf("\n");
}
*/
