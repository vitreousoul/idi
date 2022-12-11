#define CharIsStartOfBoolean(Char) ((Char) == 't' || (Char) == 'f')
#define GetChar(Buffer, Parser) ((Buffer)->Data[(Parser)->Index])

static json_token_list *CreateTokenListHead()
{
    json_token_list *Result = malloc(sizeof(json_token_list));
    Result->Type = json_token_type_Empty;
    Result->Range.Start = 0;
    Result->Range.End = 0;
    Result->Next = 0;

    return(Result);
}

static json_token_list *AppendList(json_token_list *List, json_token_type Type, json_buffer_range Range)
{
    json_token_list *Result = malloc(sizeof(json_token_list));
    Result->Type = Type;
    Result->Range = Range;
    Result->Next = 0;

    List->Next = Result;

    return(Result);
}

static json_token_list *AppendListWithTokenType(json_token_list *List, json_token_type Type)
{
    json_buffer_range Range = {0,0};
    json_token_list *Result = AppendList(List, Type, Range);

    return(Result);
}

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
    json_buffer_range Result;
    Result.Start = Parser->Index;
    Result.End = Result.Start;

    if(GetChar(Buffer, Parser) == '"')
    {
        Parser->Index++;

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

    Result.End = Parser->Index - 1;

    return(Result);
}

static json_token_list *ParseJsonBuffer(buffer *Buffer)
{
    json_token_list *Result = CreateTokenListHead();
    json_token_list *Last = Result;
    json_parser Parser = { 0, json_parser_state_Running };

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
            Last = AppendListWithTokenType(Last, json_token_type_OpenCurly);
            Parser.Index++;
        } break;
        case '}':
        {
            Last = AppendListWithTokenType(Last, json_token_type_CloseCurly);
            Parser.Index++;
        } break;
        case '[':
        {
            Last = AppendListWithTokenType(Last, json_token_type_OpenSquare);
            Parser.Index++;
        } break;
        case ']':
        {
            Last = AppendListWithTokenType(Last, json_token_type_CloseSquare);
            Parser.Index++;
        } break;
        case ',':
        {
            Last = AppendListWithTokenType(Last, json_token_type_Comma);
            Parser.Index++;
        } break;
        case ':':
        {
            Last = AppendListWithTokenType(Last, json_token_type_Colon);
            Parser.Index++;
        } break;
        case '"':
        {
            json_buffer_range Range = ParseString(Buffer, &Parser);

            if(Range.End == Range.Start)
            {
                ParserError(&Parser, "Invalid string token range");
            }

            Last = AppendList(Last, json_token_type_String, Range);
            Parser.Index++;
        } break;
        default:
        {
            /* TODO: break up the logic for numbers and booleans */
            json_token_type TokenType = json_token_type_Empty;
            json_buffer_range TokenRange = {0,0};

            if(CharIsDigit(Buffer->Data[Parser.Index]))
            {
                TokenRange = ParseNumber(Buffer, &Parser);

                if(TokenRange.End > TokenRange.Start)
                {
                    TokenType = json_token_type_Number;
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
                Last = AppendList(Last, TokenType, TokenRange);
            }
            else
            {
                PrintError("default error\n");
                Parser.State = json_parser_state_Error;
            }
        } break;
        }
    }

    return(Result);
}

static json_value *ParseJsonTokens(json_token_parser *Parser)
{
    json_value *Result = malloc(sizeof(json_value));

    while(Parser->State == json_parser_state_Running && Parser->Token != 0)
    {
        switch(Parser->Token->Type)
        {
        case json_token_type_True:
        {
            Result->Type = json_value_Boolean;
            Result->Value.Boolean = True;
            Parser->Token = Parser->Token->Next;
            Parser->State = json_parser_state_Error;
        } break;
        case json_token_type_False:
        {
            Result->Type = json_value_Boolean;
            Result->Value.Boolean = False;
            Parser->Token = Parser->Token->Next;
            Parser->State = json_parser_state_Error;
        } break;
        case json_token_type_String:
        {
            Result->Type = json_value_String;
            Result->Value.Range = Parser->Token->Range;
            Parser->Token = Parser->Token->Next;
            Parser->State = json_parser_state_Error;
        } break;
        case json_token_type_OpenCurly:
        {
            Result->Type = json_value_Object;
            typedef enum state { Key, Value } state;
            state State = Key;
            json_object *CurrentItem = malloc(sizeof(json_object));
            CurrentItem->Value = 0;
            CurrentItem->Next = 0;
            json_object *FirstItem = CurrentItem;
            Parser->Token = Parser->Token->Next;

            while(Parser->State == json_parser_state_Running && Parser->Token != 0)
            {
                switch(State)
                {
                case Key:
                {
                    switch(Parser->Token->Type)
                    {
                    case json_token_type_String:
                    {
                        CurrentItem->Key = Parser->Token->Range;
                        Parser->Token = Parser->Token->Next;

                        if(Parser->Token && Parser->Token->Type == json_token_type_Colon)
                        {
                            State = Value;
                            Parser->Token = Parser->Token->Next;
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
                        Parser->State = json_parser_state_Error;
                        Parser->Token = Parser->Token->Next;
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
                    CurrentItem->Value = ParseJsonTokens(Parser);
                    json_object *NextItem = malloc(sizeof(json_object));
                    NextItem->Value = 0;
                    NextItem->Next = 0;
                    CurrentItem->Next = NextItem;
                    CurrentItem = NextItem;

                    if(Parser->Token && Parser->Token->Type == json_token_type_Comma)
                    {
                        Parser->Token = Parser->Token->Next;
                        continue;
                    }

                    State = Key;
                } break;
                }
            }
        } break;
        case json_token_type_OpenSquare:
        {
            // TODO: a trailing comma is required for array items to be parse correctly, so we should fix that :(
            Result->Type = json_value_Array;
            json_array *CurrentItem = malloc(sizeof(json_array));
            json_array *FirstItem = CurrentItem;
            Parser->Token = Parser->Token->Next;

            while(Parser->State == json_parser_state_Running && Parser->Token != 0)
            {
                if(Parser->Token->Type == json_token_type_CloseSquare)
                {
                    Parser->State = json_parser_state_Error;
                    Parser->Token = Parser->Token->Next;
                    Result->Value.Array = FirstItem;
                }
                else
                {
                    CurrentItem->Value = ParseJsonTokens(Parser);
                    json_array *NextItem = malloc(sizeof(json_array));
                    NextItem->Value = 0;
                    NextItem->Next = 0;
                    CurrentItem->Next = NextItem;
                    CurrentItem = NextItem;

                    if(Parser->Token && Parser->Token->Type == json_token_type_Comma)
                    {
                        Parser->Token = Parser->Token->Next;
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

static void PrintJsonValue(json_value *Value, u32 Depth)
{
    u32 I;

    if(Value == 0)
    {
        return;
    }
    for(I = 0; I < Depth; I++) printf(" ");
    switch(Value->Type)
    {
    case json_value_String: { printf("String\n"); } break;
    case json_value_Number: { printf("Number\n"); } break;
    case json_value_Boolean: { printf("Boolean\n"); } break;
    case json_value_Object:
    {
        json_object *Object = Value->Value.Object;
        printf("Object\n");
        while(Object && Object->Value)
        {
            PrintJsonValue(Object->Value, Depth + 4);
            Object = Object->Next;
        }
    } break;
    case json_value_Array:
    {
        json_array *Array = Value->Value.Array;
        printf("Array\n");
        while(Array)
        {
            PrintJsonValue(Array->Value, Depth + 4);
            Array = Array->Next;
        }
    }
    }
}

json_value *ParseJson(buffer *Buffer)
{
    json_token_list *Token = ParseJsonBuffer(Buffer);
    if(Token && Token->Type == json_token_type_Empty && Token->Next)
    {
        Token = Token->Next;
    }
    json_token_parser Parser;
    Parser.Token = Token;
    Parser.State = json_parser_state_Running;

    json_value *Result = ParseJsonTokens(&Parser);
    PrintJsonValue(Result, 0);

    return(Result);
}
