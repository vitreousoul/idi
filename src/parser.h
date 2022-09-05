#ifndef PARSER_H_
#define PARSER_H_

#include "types.h"
#include "buffer.h"

typedef enum parse_tree_state
{
ParseTreeStateRunning,
ParseTreeStateSuccess,
ParseTreeStateError,
} parse_tree_state;

typedef enum parse_tree_type
{
ParseTreeTypeTextMatch,
ParseTreeTypeCharSet,
ParseTreeTypeAnd,
ParseTreeTypeOr,
} parse_tree_type;

typedef struct text_match_node
{
    buffer *Text;
    size Index;
} text_match_node;

typedef struct char_set_node
{
    buffer *Text;
} char_set_node;

typedef struct parse_tree
{
    parse_tree_type Type;
    parse_tree_state State;
    u32 NodeCount;
    union
    {
        text_match_node *TextMatch;
        char_set_node *CharSet;
        struct parse_tree *Nodes;
    } Value;
} parse_tree;

typedef struct parser
{
    size Index;
} parser;

const char *DisplayParseTreeState(parse_tree *ParseTree);
parse_tree_state GetParseTreeState(parse_tree *ParseTree);
parse_tree ParseBuffer(buffer *Buffer);

#endif // PARSER_H_
