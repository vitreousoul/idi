#ifndef PARSER_H_
#define PARSER_H_

#include "types.h"
#include "buffer.h"

static const u32 MAX_STATE_ARRAY_COUNT = 128;

enum token_type {
TokenTypeStreamBegin,
TokenTypeKeywordBind,
TokenTypeVariable,
TokenTypeCurlyOpen,
TokenTypeCurlyClose,
TokenTypeStreamEnd,
TokenTypeError,
};
typedef enum token_type token_type;

struct parser {
    size At;
    u32 State[MAX_STATE_ARRAY_COUNT];
    u32 StatePosition;
};
typedef struct parser parser;

struct token_list {
    token_type Type;
    buffer *Text;
    struct token_list *Next;
};
typedef struct token_list token_list;

struct token_range {
    size Begin;
    size End;
};
typedef struct token_range token_range;

token_list *Parse(buffer Buffer);

#endif // PARSER_H_
