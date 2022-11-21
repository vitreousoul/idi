#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "types.h"
#include "buffer.h"
#include "json_parser.h"
#include "platform.h"
#include "gui.h"

#define ArrayItemSize(Array) (sizeof(Array[0]))
#define ArrayCount(Array) (sizeof(Array) / ArrayItemSize(Array))

#define CharIsNullChar(Char) ((Char) == '\0')
#define CharIsSpace(Char) (((Char) == ' ') || ((Char) == '\n') || ((Char) == '\r') || ((Char) == '\t'))
#define CharIsDigit(Char) (((Char) >= '0') && ((Char) <= '9'))
#define CharIsAlphaLower(Char) ((Char) >= 'a' && (Char) <= 'z')
#define CharIsAlphaUpper(Char) ((Char) >= 'A' && (Char) <= 'Z')
#define CharIsAlpha(Char) (CharIsAlphaLower(Char) || CharIsAlphaUpper(Char))
