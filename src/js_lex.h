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
    token_kind_Query = '?',
    token_kind_Comma = ',',
    token_kind_Period = '.',
    token_kind_Plus = '+',
    token_kind_Minus = '-',
    token_kind_Star = '*',
    token_kind_Equals = '=',

    token_kind_String = 128,
    token_kind_Keyword = 129,
    token_kind_Integer = 130,
    token_kind_Float = 131,
    token_kind_Identifier = 132,
    token_kind_DoubleEquals = 133,
    token_kind_TripleEquals = 134,
    token_kind_Arrow = 135,
    // reserved keywords
    token_kind_Import = 136,
    token_kind_Export = 137,
    token_kind_From = 138,
    token_kind_Var = 139,
    token_kind_Let = 140,
    token_kind_Const = 141,
    token_kind_Function = 142,
    token_kind_Return = 143,
    token_kind_As = 144,
    token_kind_Default = 145,
} token_kind;

typedef struct
{
    token_kind TokenKind;
    char* String;
} KeywordKind;

typedef enum
{
    identifier_kind_Unresolved,
    identifier_kind_Keyword,
} identifier_kind;

/* typedef struct */
/* { */
/*     size Start; */
/*     size End; */
/* } range; */

typedef struct token
{
    token_kind Kind;
    union
    {
        char *String;
        identifier_kind IdentifierKind;
    };
} token;

typedef struct lexer
{
    buffer Source;
    size I;
} lexer;

typedef struct
{
    u32 Capacity;
    u32 At;
    char *Data;
} string_buffer;

u32 TestJsLex(void);
token *LexJs(lexer *Lexer);
