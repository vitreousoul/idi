typedef enum json_token_type
{
    // TODO: define explicit enum values once values stop changing
    json_token_type_Empty,
    json_token_type_OpenCurly,
    json_token_type_CloseCurly,
    json_token_type_OpenSquare,
    json_token_type_CloseSquare,
    json_token_type_Comma,
    json_token_type_Colon,
    json_token_type_String,
    json_token_type_Number,
    json_token_type_True,
    json_token_type_False,
} json_token_type;

typedef struct json_token_range
{
    size Start;
    size End;
} json_token_range;

typedef struct json_token_list
{
    json_token_type Type;
    json_token_range Range;
    struct json_token_list *Next;
} json_token_list;

typedef enum json_parser_state
{
    json_parser_state_Running = 0,
    json_parser_state_Error = 1,
    json_parser_state_EndOfSource = 2,
} json_parser_state;

typedef struct json_parser
{
    size Index;
    json_parser_state State;
} json_parser;

json_token_list *ParseJson(buffer *Buffer);
char *GetJsonTokenTypeString(json_token_type Type);

char *
GetJsonTokenTypeString(json_token_type Type)
{
    switch(Type)
    {
        case json_token_type_Empty: { return "Empty"; } break;
        case json_token_type_OpenCurly: { return "OpenCurly"; } break;
        case json_token_type_CloseCurly: { return "CloseCurly"; } break;
        case json_token_type_OpenSquare: { return "OpenSquare"; } break;
        case json_token_type_CloseSquare: { return "CloseSquare"; } break;
        case json_token_type_Comma: { return "Comma"; } break;
        case json_token_type_Colon: { return "Colon"; } break;
        case json_token_type_String: { return "String"; } break;
        case json_token_type_Number: { return "Number"; } break;
        case json_token_type_True: { return "True"; } break;
        case json_token_type_False: { return "False"; } break;
    }
}
