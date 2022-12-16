#!/usr/bin/env bash

DEBUG=0
SOURCE_FILES="src/idi.c"
LIBS="`sdl2-config --cflags --libs`"
SETTINGS="-std=c89 -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wno-comment"

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

echo $TARGET
echo $SETTINGS
echo $LIBS
echo $SOURCE_FILES

gcc $TARGET $SETTINGS $LIBS $SOURCE_FILES
