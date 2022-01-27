# Use this until a proper Makefile is written
go() {
    gcc -g parser.c helpers.c wav.c -o wav
    if [ $? -eq 0 ]
    then
        ./wav
        printf "\n$?"
    fi
    echo ""
}

db() {
    gcc -g parser.c helpers.c wav.c -o wav
    if [ $? -eq 0 ]
    then
        gdb wav
        printf "\n$?"
    fi
    echo ""
}

# dumps bytes in data section of the dataChunk
ddata() {
    xxd -s 78 -o -78 sample.wav
}
