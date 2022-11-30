#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define BUILD_GUI 0

#if BUILD_GUI
#include <SDL2/SDL.h>
#endif

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "types.h"
#include "buffer.h"
#include "json_parser.h"
#include "platform.h"
#if BUILD_GUI
#include "gui.h"
#endif

#define ArrayItemSize(Array) (sizeof(Array[0]))
#define ArrayCount(Array) (sizeof(Array) / ArrayItemSize(Array))

#define CharIsNullChar(Char) ((Char) == '\0')
#define CharIsSpace(Char) (((Char) == ' ') || ((Char) == '\n') || ((Char) == '\r') || ((Char) == '\t'))
#define CharIsDigit(Char) (((Char) >= '0') && ((Char) <= '9'))
#define CharIsAlphaLower(Char) ((Char) >= 'a' && (Char) <= 'z')
#define CharIsAlphaUpper(Char) ((Char) >= 'A' && (Char) <= 'Z')
#define CharIsAlpha(Char) (CharIsAlphaLower(Char) || CharIsAlphaUpper(Char))
