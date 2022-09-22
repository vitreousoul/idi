#!/usr/bin/env bash

DEBUG=0
SOURCE_FILES="src/idi.c"
SETTINGS="-std=c99 -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wdeclaration-after-statement"

if [ $DEBUG -eq 0 ]; then
    echo "Optimized build";
    TARGET="-o build/idi.exe"
else
    echo "Debug build";
    TARGET="-g3 -O0"
fi

gcc $TARGET $SETTINGS $SOURCE_FILES
