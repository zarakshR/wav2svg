#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define BYTE uint8_t

int main(){

    FILE* file = fopen("sample3.wav", "rb");
    BYTE magic[4] = {0x52, 0x49, 0x46, 0x46};
    BYTE sig[4];
    fread(&sig, sizeof(BYTE), 4, file);

    // for (int i = 0; i < 4; ++i) {
    //     printf("%x", magic[i]);
    // }
    // printf("\n");
    // for (int i = 0; i < 4; ++i) {
    //     printf("%x", sig[i]);
    // }

    if ((memcmp(sig, magic, sizeof(sig))) != 0) {return 1;}
    return 1;
}
