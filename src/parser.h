#ifndef PARSER_H_
#define PARSER_H_

#include "types.h"
#include "buffer.h"

static const u32 MAX_STATE_ARRAY_COUNT = 128;

typedef enum parser_state
{
ParserStateTopLevel,
ParserStateBind,
ParserStateBindName,
ParserStateVariable,
ParserStateBindBlockOpen,
ParserStateBindBlockClose,
ParserStateBindStatements,
ParserStateBindStatementOperator,
ParserStateBindExpression,
ParserStateMatch,
ParserStateEnd,
} parser_state;

typedef enum token_type
{
TokenTypeStreamBegin,
TokenTypeKeywordBind,
TokenTypeOperatorEqual,
TokenTypeVariable,
TokenTypeCurlyOpen,
TokenTypeCurlyClose,
TokenTypeStreamEnd,
TokenTypeError,
} token_type;

typedef struct parser
{
    size At;
    u32 State[MAX_STATE_ARRAY_COUNT];
    u32 StatePosition;
} parser;

typedef struct token_list
{
    token_type Type;
    buffer *Text;
    struct token_list *Next;
} token_list;

typedef struct token_range
{
    size Begin;
    size End;
} token_range;

token_list *Parse(buffer Buffer);

#endif // PARSER_H_
