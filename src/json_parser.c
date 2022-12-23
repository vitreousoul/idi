#define CharIsStartOfBoolean(Char) ((Char) == 't' || (Char) == 'f')
#define GetChar(Buffer, Parser) ((Buffer)->Data[(Parser)->Index])

#define MAX_JSON_TOKEN_CHUNK_COUNT 1 << 12
json_token Tokens[MAX_JSON_TOKEN_CHUNK_COUNT];

static void ParserError(json_parser *Parser, char *Message)

{
    PrintError(Message);
    Parser->State = json_parser_state_Error;
}

static void ParseSpace(buffer *Buffer, json_parser *Parser)
{
    while(CharIsSpace(Buffer->Data[Parser->Index]) && (Parser->Index < Buffer->Size))
    {
        Parser->Index++;
    }
}

static void ParseKeyword(buffer *Buffer, json_parser *Parser, char *Keyword)
{
    u32 KeywordIndex = 0;
    b32 Matched = True;

    for(;;)
    {
        if(CharIsNullChar(Keyword[KeywordIndex]))
        {
            break;
        }
        else if(GetChar(Buffer, Parser) == Keyword[KeywordIndex])
        {
            Parser->Index++;
            KeywordIndex++;
        }
        else
        {
            Matched = False;
        }
    }

    if(!Matched)
    {
        ParserError(Parser, "Error matching keyword");
    }
}

static json_buffer_range ParseString(buffer *Buffer, json_parser *Parser)
{
    json_buffer_range Result = {0,0};

    if(GetChar(Buffer, Parser) == '"')
    {
        Parser->Index++;
        Result.Start = Parser->Index;
        Result.End = Result.Start;

        for(;;)
        {
            if(Parser->Index >= Buffer->Size)
            {
                break;
            }
            else if(GetChar(Buffer, Parser) == '"')
            {
                Result.End = Parser->Index;
                break;
            }
            else
            {
                size Increment = GetChar(Buffer, Parser) == '\\' ? 2 : 1;
                Parser->Index += Increment;
            }
        }
    }

    return(Result);
}

static json_token_type ParseBoolean(buffer *Buffer, json_parser *Parser)
{
    json_token_type Result = json_token_type_Empty;

    switch(GetChar(Buffer, Parser))
    {
    case 't':
    {
        ParseKeyword(Buffer, Parser, "true");

        if(Parser->State != json_parser_state_Error)
        {
            Result = json_token_type_True;
        }
    } break;
    case 'f':
    {
        ParseKeyword(Buffer, Parser, "false");

        if(Parser->State != json_parser_state_Error)
        {
            Result = json_token_type_False;
        }
    } break;
    default:
    {
        ParserError(Parser, "parsing boolean");
    }
    }

    return(Result);
}

static json_buffer_range ParseNumber(buffer *Buffer, json_parser *Parser)
{
    json_buffer_range Result = {Parser->Index, Parser->Index};
    b32 PreFloatPoint = True;
    u32 Iter = 0;

    for(;;)
    {
        if(Iter++ > 32) { PrintError("ParseNumber max iter"); break; }

        if(Parser->Index >= Buffer->Size)
        {
            break;
        }
        else if(PreFloatPoint && (GetChar(Buffer, Parser) == '.'))
        {
            PreFloatPoint = False;
            Parser->Index++;
        }
        else if(CharIsDigit(GetChar(Buffer, Parser)))
        {
            Parser->Index++;
        }
        else
        {
            break;
        }
    }
    Result.End = Parser->Index;
    /* Parser->State = json_parser_state_Success; */

    return(Result);
}

static u32 ParseJsonBuffer(buffer *Buffer)
{
    json_parser Parser = { 0, json_parser_state_Running };
    u32 TokenIndex = 0;

    while(Parser.State == json_parser_state_Running)
    {
        ParseSpace(Buffer, &Parser);

        if(Parser.Index >= Buffer->Size)
        {
            /* end of buffer */
            break;
        }

        switch(Buffer->Data[Parser.Index])
        {
        case '{':
        {
            Tokens[TokenIndex].Type = json_token_type_OpenCurly;
            ++TokenIndex;
            ++Parser.Index;
        } break;
        case '}':
        {
            Tokens[TokenIndex].Type = json_token_type_CloseCurly;
            ++TokenIndex;
            ++Parser.Index;
        } break;
        case '[':
        {
            Tokens[TokenIndex].Type = json_token_type_OpenSquare;
            ++TokenIndex;
            ++Parser.Index;
        } break;
        case ']':
        {
            Tokens[TokenIndex].Type = json_token_type_CloseSquare;
            ++TokenIndex;
            ++Parser.Index;
        } break;
        case ',':
        {
            Tokens[TokenIndex].Type = json_token_type_Comma;
            ++TokenIndex;
            ++Parser.Index;
        } break;
        case ':':
        {
            Tokens[TokenIndex].Type = json_token_type_Colon;
            ++TokenIndex;
            ++Parser.Index;
        } break;
        case '"':
        {
            json_buffer_range Range = ParseString(Buffer, &Parser);

            if(Range.End == Range.Start)
            {
                ParserError(&Parser, "Invalid string token range");
            }

            Tokens[TokenIndex].Type = json_token_type_String;
            Tokens[TokenIndex].Range = Range;
            ++TokenIndex;
            ++Parser.Index;
        } break;
        default:
        {
            /* TODO: break up the logic for numbers and booleans */
            json_token_type TokenType = json_token_type_Empty;
            json_buffer_range TokenRange = {0,0};

            if(CharIsDigit(Buffer->Data[Parser.Index]) || Buffer->Data[Parser.Index] == '.')
            {
                TokenRange = ParseNumber(Buffer, &Parser);
                if(TokenRange.End > TokenRange.Start)
                {
                    TokenType = json_token_type_Number;
                    Tokens[TokenIndex].Range = TokenRange;
                }
            }
            else if(CharIsStartOfBoolean(Buffer->Data[Parser.Index]))
            {
                TokenType = ParseBoolean(Buffer, &Parser);
            }
            else
            {
                Parser.State = json_parser_state_Error;
            }

            if((Parser.State != json_parser_state_Error) &&
               (TokenType != json_token_type_Empty))
            {
                Tokens[TokenIndex].Type = TokenType;
                ++TokenIndex;
            }
            else
            {
                PrintError("default error\n");
                Parser.State = json_parser_state_Error;
            }
        } break;
        }
    }

    return(TokenIndex);
}

static b32 ValidJsonToken(json_token Token)
{
    b32 Result = (Token.Type > 0 && Token.Type <= 10);

    return(Result);
}

static json_value *ParseJsonTokens(json_parser *Parser, u32 TokenCount)
{
    json_value *Result = malloc(sizeof(json_value));

    while(Parser->State == json_parser_state_Running &&
          ValidJsonToken(Tokens[Parser->Index]) &&
          Parser->Index < TokenCount)
    {
        switch(Tokens[Parser->Index].Type)
        {
        case json_token_type_True:
        {
            Result->Type = json_value_Boolean;
            Result->Value.Boolean = True;
            ++Parser->Index;
            Parser->State = json_parser_state_Success;
        } break;
        case json_token_type_False:
        {
            Result->Type = json_value_Boolean;
            Result->Value.Boolean = False;
            ++Parser->Index;
            Parser->State = json_parser_state_Success;
        } break;
        case json_token_type_String:
        {
            Result->Type = json_value_String;
            Result->Value.Range = Tokens[Parser->Index].Range;
            ++Parser->Index;
            Parser->State = json_parser_state_Success;
        } break;
        case json_token_type_Number:
        {
            Result->Type = json_value_Number;
            Result->Value.Range = Tokens[Parser->Index].Range;
            ++Parser->Index;
            Parser->State = json_parser_state_Success;
        } break;
        case json_token_type_OpenCurly:
        {
            Result->Type = json_value_Object;
            typedef enum state { Key, Value } state;
            state State = Key;
            json_object *CurrentItem = malloc(sizeof(json_object));
            CurrentItem->Value = 0;
            CurrentItem->Next = 0;
            Result->Value.Object = CurrentItem;
            ++Parser->Index;

            while(Parser->State == json_parser_state_Running && ValidJsonToken(Tokens[Parser->Index]))
            {
                switch(State)
                {
                case Key:
                {
                    switch(Tokens[Parser->Index].Type)
                    {
                    case json_token_type_String:
                    {
                        CurrentItem->Key = Tokens[Parser->Index].Range;
                        ++Parser->Index;

                        if(Tokens[Parser->Index].Type == json_token_type_Colon)
                        {
                            State = Value;
                            ++Parser->Index;
                        }
                        else
                        {
                            PrintError("Expected colon after key in json object");
                            Result = 0;
                            Parser->State = json_parser_state_Error;
                            break;
                        }
                    } break;
                    case json_token_type_CloseCurly:
                    {
                        Parser->State = json_parser_state_Success;
                        ++Parser->Index;
                    } break;
                    default: {
                        PrintError("Expected string for json object key or close-curly");
                        Result = 0;
                        Parser->State = json_parser_state_Error;
                        break;
                    }
                    }
                } break;
                case Value:
                {
                    CurrentItem->Value = ParseJsonTokens(Parser, TokenCount);
                    json_object *NextItem = malloc(sizeof(json_object));
                    NextItem->Value = 0;
                    NextItem->Next = 0;
                    CurrentItem->Next = NextItem;
                    CurrentItem = NextItem;

                    if(Tokens[Parser->Index].Type == json_token_type_Comma)
                    {
                        ++Parser->Index;
                        continue;
                    }

                    State = Key;
                } break;
                }
            }
        } break;
        case json_token_type_OpenSquare:
        {
            Result->Type = json_value_Array;
            json_array *CurrentItem = malloc(sizeof(json_array));
            CurrentItem->Value = 0;
            CurrentItem->Next = 0;
            Result->Value.Array = CurrentItem;
            ++Parser->Index;

            while(Parser->State == json_parser_state_Running && ValidJsonToken(Tokens[Parser->Index]))
            {
                if(Tokens[Parser->Index].Type == json_token_type_CloseSquare)
                {
                    Parser->State = json_parser_state_Success;
                    ++Parser->Index;
                }
                else
                {
                    CurrentItem->Value = ParseJsonTokens(Parser, TokenCount);
                    json_array *NextItem = malloc(sizeof(json_array));
                    NextItem->Value = 0;
                    NextItem->Next = 0;
                    CurrentItem->Next = NextItem;
                    CurrentItem = NextItem;

                    if(Tokens[Parser->Index].Type == json_token_type_Comma)
                    {
                        ++Parser->Index;
                        continue;
                    }
                }
            }
        } break;
        default:
        {
            PrintError("Unexpected token while parsing json value");
            Parser->State = json_parser_state_Error;
        }
        }
    }

    // NOTE: we use a "global" running value that is packed inside the parser,
    // and upon exit we have to set it back to running. This feels really hacky and should
    // probably be changed!
    Parser->State = json_parser_state_Running;

    return(Result);
}

static void PrintJsonValue(buffer *Buffer, json_value *Value, u32 Depth)
{
    u32 I;

    if(Value == 0)
    {
        return;
    }
    for(I = 0; I < Depth; I++) printf(" ");
    switch(Value->Type)
    {
    case json_value_String:
    {
        u32 CharCount = Value->Value.Range.End - Value->Value.Range.Start;
        u8 StringValue[CharCount + 1];
        StringValue[CharCount] = NULL_CHAR;
        memcpy(StringValue, Buffer->Data + Value->Value.Range.Start, CharCount);
        printf("String %s\n", StringValue);
    } break;
    case json_value_Number: {
        u32 CharCount = Value->Value.Range.End - Value->Value.Range.Start;
        u8 StringValue[CharCount + 1];
        StringValue[CharCount] = NULL_CHAR;
        memcpy(StringValue, Buffer->Data + Value->Value.Range.Start, CharCount);
        printf("Number %s\n", StringValue);
    } break;
    case json_value_Boolean: {
        printf("Boolean %d\n", Value->Value.Boolean);
    } break;
    case json_value_Object:
    {
        json_object *Object = Value->Value.Object;
        printf("Object\n");
        while(Object && Object->Value)
        {
            PrintJsonValue(Buffer, Object->Value, Depth + 4);
            Object = Object->Next;
        }
    } break;
    case json_value_Array:
    {
        json_array *Array = Value->Value.Array;
        printf("Array\n");
        while(Array)
        {
            PrintJsonValue(Buffer, Array->Value, Depth + 4);
            Array = Array->Next;
        }
    }
    }
}

json_value *ParseJson(buffer *Buffer)
{
    u32 TokenCount = ParseJsonBuffer(Buffer);

    json_parser Parser;
    Parser.State = json_parser_state_Running;
    Parser.Index = 0;

    json_value *Result = ParseJsonTokens(&Parser, TokenCount);
    PrintJsonValue(Buffer, Result, 0);

    return(Result);
}
