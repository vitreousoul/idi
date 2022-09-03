#!/usr/bin/env bash

DEBUG=0
SOURCE_FILES="src/idi.c src/parser.c src/buffer.c"

if [ $DEBUG -eq 0 ]; then
    echo "Optimized build";
    TARGET="-o build/idi.exe"
else
    echo "Debug build";
    TARGET="-g3 -O0"
fi

gcc $TARGET $SOURCE_FILES
