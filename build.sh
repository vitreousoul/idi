#!/usr/bin/env bash

DEBUG=1
GUI=0
SOURCE_FILES="src/idi.c"
SETTINGS="-std=c89 -Wall -Wextra -Wstrict-prototypes -Wold-style-definition -Wmissing-prototypes -Wmissing-declarations -Wno-comment"
GUI_LIBS=""

if [ $GUI -eq 1 ]; then
    GUI_DEFINES="-D BUILD_GUI"
    GUI_LIBS="`sdl2-config --cflags --libs` -framework OpenGL"
fi

if [ $DEBUG -eq 0 ]; then
    echo "Optimized build";
    TARGET="-O2 -o build/idi.exe"
elif [ $DEBUG -eq 1 ]; then
    echo "Debug build";
    TARGET="-g3 -O0 -o build/a.out"
elif [ $DEBUG -eq 2 ]; then
    echo "Outputting object file";
    TARGET="-c -O2"
fi

echo $TARGET
echo $SETTINGS
echo $GUI_DEFINES
echo $GUI_LIBS
echo $SOURCE_FILES

gcc $TARGET $SETTINGS $GUI_DEFINES $GUI_LIBS $SOURCE_FILES
