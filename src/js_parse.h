typedef struct
{
    s32 TokenCount;
    s32 I;
    token *Tokens;
    b32 Emit;
} js_parser;

void TestParseJs(void);
