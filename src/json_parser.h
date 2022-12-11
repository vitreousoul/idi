typedef enum json_token_type
{
    /* TODO: define explicit enum values once values stop changing */
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

typedef struct json_buffer_range
{
    size Start;
    size End;
} json_buffer_range;

typedef struct json_token_list
{
    json_token_type Type;
    json_buffer_range Range;
    struct json_token_list *Next;
} json_token_list;

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
} json_parser_state;

typedef enum json_parser_array_state
{
    json_parser_array_state_Value = 0,
    json_parser_array_state_Comma = 1,
} json_parser_array_state;

typedef struct json_parser
{
    size Index;
    json_parser_state State;
} json_parser;

typedef enum json_parse_stack_item_state
{
    json_parse_stack_item_state_Initial,
    json_parse_stack_item_state_ObjectKey,
    json_parse_stack_item_state_ObjectValue,
    json_parse_stack_item_state_ObjectRepeat,
    json_parse_stack_item_state_ArrayKey,
    json_parse_stack_item_state_ArrayValue,
} json_parse_stack_item_state;

typedef struct json_token_parser
{
    json_token_list *Token;
    json_parser_state State;
} json_token_parser;


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
