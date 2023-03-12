// c libs
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

// code
#include "types.h"
#include "common.h"
#include "ds.h"
#include "vec.h"
#include "buffer.h"
#include "platform.h"
#include "json_parser.h"
#include "js_lex.h"
#include "js_parse.h"

// NOTE: BUILD_GUI is used to toggle the GUI build path. Currently the BUILD_GUI path is the
// only path containing library code, which may be of interest when building this project.
#ifdef BUILD_GUI
// gui libs
#include <SDL.h>
#include <SDL_opengl.h>
#define STB_TRUETYPE_IMPLEMENTATION
#include "stb/stb_truetype.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
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
#define CharIsAlphaNum(Char) (CharIsAlpha(Char) || CharIsDigit(Char))
