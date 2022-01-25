# Use this until a proper Makefile is built

go() {
    gcc -g -o wav wav.c
    ./wav
    printf "\n$?"
    echo ""
}

db() {
    gdb wav
}
