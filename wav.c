#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BYTE uint8_t
#define BYTE_SIZE sizeof(BYTE)
#define RIFF_CHUNK_HEADER_SIZE 12

// chunkSize never includes the 8 bytes from chunkID and itself
// Everything except chunkIDs are little-endian
// PCM data is two's-complement except for resolutions of 1-8 bits, which are
// represented as offset binary.

typedef struct {    // 24 bytes in length.
  BYTE chunkID[4];  // Should be "fmt ". Note the trailing space.
  BYTE chunkSize[4];
  BYTE formatCode[2];     // Should be 0x10 0x00 for PCM data
  BYTE channels[2];       // No. of channels -- Also blocks per sample
  BYTE sampleRate[4];     // Samples per second
  BYTE byteRate[4];       // Bytes per second
  BYTE blockAlign[2];     // Bytes per block
  BYTE bitsPerSample[2];  // Bits per sample = 8 * Bytes per sample
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
  FILE* input_file = fopen("sample.wav", "rb");
  if (input_file == NULL) {
    return 4;
  }

  RiffChunk riff_chunk;

  // Read RIFF chunk header
  fread(&riff_chunk, RIFF_CHUNK_HEADER_SIZE, 1, input_file);
  // Read format chunk
  fread(&riff_chunk.fmtChunk, sizeof(riff_chunk.fmtChunk), 1, input_file);

  // This is the format code for PCM encoding
  BYTE fmt_pcm_sig[2] = {0x10, 0x00};

  BYTE signal = 0;
  // Check if RIFF
  signal += (memcmp(riff_chunk.chunkID, "RIFF", BYTE_SIZE * 4));
  // Check if WAVE
  signal += (memcmp(riff_chunk.format, "WAVE", BYTE_SIZE * 4));
  // Check if using PCM encoding
  signal += (memcmp(riff_chunk.fmtChunk.formatCode, fmt_pcm_sig,
                    sizeof(fmt_pcm_sig)));

  if (signal != 0) {
    return 1;
  }
  // File format checking done.

  // We have to scan for the data chunk manually because there can be any number
  // of undefined extra tags before or after the data chunk.

  // Read file until "data" bytes are found
  // We have to read 4 bytes starting from every byte because the chunk ID bytes
  // may not be 4-aligned due to the undefined tags not being a multiple of 4 in
  // length like below -
  //
  // [______1ST-READ______]  [_____2ND-READ______]
  // |                    | |                    |
  // [0x00] [0x00] [d] [a]  [t] [a] [0x00] [0x00]
  //               ^^^^^^^^T^^^^^^^
  //                       |____________ "data" is not 4-aligned
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

  fread(&riff_chunk.dataChunk.chunkSize, 4, 1, input_file);
  uint32_t data_size = 0;
  for (int8_t _i = 3; _i >= 0; _i--) {  // Calculate size of data in bytes
    data_size = data_size << 8 | riff_chunk.dataChunk.chunkSize[_i];
  }

  riff_chunk.dataChunk.data = (BYTE*)malloc(data_size);
  if (riff_chunk.dataChunk.data == NULL) {
    return 2;
  }

  // Populate data
  fread(riff_chunk.dataChunk.data, data_size, 1, input_file);
  if (feof(input_file) || ferror(input_file)) {
    return 3;
  }

  free(riff_chunk.dataChunk.data);
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
  printf("%02x", *(riff_chunk.dataChunk.data + pos));
  printf("\n");
}
*/
