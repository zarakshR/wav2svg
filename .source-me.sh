# Use this until a proper Makefile is written

go() {
    gcc -g -o wav wav.c
    ./wav
    printf "\n$?"
    echo ""
}

db() {
    gdb wav
}

# dumps all bytes in "sample.wav"
dbytes() {
    gcc printbytes.c -o printbytes
    ./printbytes
}

# dumps bytes in data section of the dataChunk
ddata() {
    xxd -s 78 -o -78 sample.wav
}
