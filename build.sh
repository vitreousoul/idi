#!/usr/bin/env bash

DEBUG=0
SOURCE_FILES="src/idi.c"
SETTINGS="-std=c99 -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement"

if [ $DEBUG -eq 0 ]; then
    echo "Optimized build";
    TARGET="-O2 -o build/idi.exe"
elif [ $DEBUG -eq 1 ]; then
    echo "Debug build";
    TARGET="-g3 -O0"
elif [ $DEBUG -eq 2 ]; then
    echo "Outputting object file";
    TARGET="-c -O2"
fi

gcc $TARGET $SETTINGS $SOURCE_FILES
