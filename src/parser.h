#ifndef PARSER_H_
#define PARSER_H_


#include "types.h"

static const u32 MAX_STATE_ARRAY_COUNT = 128;

struct parser {
    size At;
    u32 State[MAX_STATE_ARRAY_COUNT];
    u32 StatePosition;
};
typedef struct parser parser;

struct token_list {
    u32 Type;
    buffer Text;
    struct token_list *Next;
};
typedef struct token_list token_list;

token_list *Parse(buffer Buffer);

#endif // PARSER_H_
