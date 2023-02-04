typedef enum token_kind
{
    token_kind_None = 0,
    token_kind_CurlyOpen = '{',
    token_kind_CurlyClose = '}',
    token_kind_SquareOpen = '[',
    token_kind_SquareClose = ']',
    token_kind_ParenOpen = '(',
    token_kind_ParenClose = ')',
    token_kind_AngleOpen = '<',
    token_kind_AngleClose = '>',
    token_kind_BackSlash = '\\',
    token_kind_ForwardSlash = '/',
    token_kind_SemiColon = ';',
    token_kind_Colon = ':',
    token_kind_Comma = ',',
    token_kind_Period = '.',
    token_kind_Plus = '+',
    token_kind_Minus = '-',
    token_kind_Star = '*',
    token_kind_Equals = '=',

    token_kind_String = 128,
    token_kind_Integer,
    token_kind_Float,
    token_kind_Identifier,
} token_kind;

typedef struct token
{
    token_kind Kind;
    union
    {
        struct
        {
            size Start;
            size End;
        } String;
    } Value;
} token;

typedef struct lexer
{
    buffer Source;
    size I;
} lexer;

u32 TestJsLex(void);
