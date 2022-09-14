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
    ParseTreeTypeCharRange,
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
    b32 Exclusive;
    buffer *Text;
} char_set_node;

typedef struct char_range
{
    char Begin;
    char End;
} char_range;

typedef struct string_literal_node
{
    b32 InsideString;
} string_literal_node;

typedef struct parse_tree
{
    parse_tree_type Type;
    parse_tree_state State;
    u32 NodeCount;
    i32 Repeat;
    u32 RepeatCount;
    u8 RepeatEndChar;

    union
    {
        text_match_node *TextMatch;
        char_set_node *CharSet;
        string_literal_node *StringLiteral;
        char_range *CharRange;
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
