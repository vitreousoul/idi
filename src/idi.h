// c libs
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h> // NOTE: for memcpy i guess?

// code
#include "types.h"
#include "vec.h"
#include "buffer.h"
#include "platform.h"
#include "json_parser.h"
#include "js_lex.h"
#include "js_parse.h"

// NOTE: BUILD_GUI is used to toggle the GUI build path. Currently the BUILD_GUI path is the
// only path containing library code, which may be of interest when building this project.
#define BUILD_GUI 0

#if BUILD_GUI
// gui libs
#include <SDL2/SDL.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"
// gui code
#include "gui.h"
#endif

#define ArrayItemSize(Array) (sizeof(Array[0]))
#define ArrayCount(Array) (sizeof(Array) / ArrayItemSize(Array))

#define NULL_CHAR '\0'
#define CharIsNullChar(Char) ((Char) == NULL_CHAR)
#define CharIsSpace(Char) (((Char) == ' ') || ((Char) == '\n') || ((Char) == '\r') || ((Char) == '\t'))
#define CharIsDigit(Char) (((Char) >= '0') && ((Char) <= '9'))
#define CharIsAlphaLower(Char) ((Char) >= 'a' && (Char) <= 'z')
#define CharIsAlphaUpper(Char) ((Char) >= 'A' && (Char) <= 'Z')
#define CharIsAlpha(Char) (CharIsAlphaLower(Char) || CharIsAlphaUpper(Char))
