#include "defs.h"
#include <stdint.h>

// Returns sum of first N bytes at seriesofBytes
uint64_t sumNBytesFrom(const BYTE* seriesOfBytes, uint32_t N)
{
    if (seriesOfBytes == NULL) { return 0; }

    uint64_t sum = 0;
    for (int64_t _i = (N - 1); _i >= 0; _i--) {
        sum = sum << 8 | *(seriesOfBytes + _i);
    }
    return sum;
}
