typedef enum json_token_type
{
    json_token_type_Empty = 0,
    json_token_type_OpenCurly = 1,
    json_token_type_CloseCurly = 2,
    json_token_type_OpenSquare = 3,
    json_token_type_CloseSquare = 4,
    json_token_type_Comma = 5,
    json_token_type_Colon = 6,
    json_token_type_String = 7,
    json_token_type_Number = 8,
    json_token_type_True = 9,
    json_token_type_False = 10,
} json_token_type;

typedef struct json_buffer_range
{
    size Start;
    size End;
} json_buffer_range;

typedef struct json_token
{
    json_token_type Type;
    json_buffer_range Range;
} json_token;

typedef enum json_value_type
{
    json_value_Object,
    json_value_Array,
    json_value_String,
    json_value_Number,
    json_value_Boolean,
} json_value_type;

typedef struct json_value json_value;

typedef struct json_object
{
    json_buffer_range Key;
    json_value *Value;
    struct json_object *Next;
} json_object;

typedef struct json_array
{
    json_value *Value;
    struct json_array *Next;
} json_array;

struct json_value
{
    json_value_type Type;
    union
    {
        b32 Boolean;
        f32 Number;
        json_object *Object;
        json_array *Array;
        json_buffer_range Range;
    } Value;
};

typedef enum json_parser_state
{
    json_parser_state_Running = 0,
    json_parser_state_Error = 1,
    json_parser_state_EndOfSource = 2,
    json_parser_state_Success = 3,
} json_parser_state;

typedef struct json_parser
{
    u32 Index;
    json_parser_state State;
} json_parser;

json_value *ParseJson(buffer *Buffer);
char *GetJsonTokenTypeString(json_token_type Type);

char *GetJsonTokenTypeString(json_token_type Type)
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
