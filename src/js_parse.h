typedef enum
{
    js_parse_node_kind_None,
} js_parse_node_kind;

typedef struct
{
    s32 TokenCount;
    s32 I;
    token *Tokens;
    b32 Emit;
} js_parser;

typedef struct
{

} js_parse_node;

void TestParseJs(void);
