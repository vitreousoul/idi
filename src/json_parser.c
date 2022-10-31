#define CharIsStartOfBoolean(Char) ((Char) == 't' || (Char) == 'f')
#define GetChar(Buffer, Parser) ((Buffer)->Data[(Parser)->Index])

static json_token_list *
CreateTokenListHead()
{
    json_token_list *Result = malloc(sizeof(json_token_list));
    Result->Type = json_token_type_Empty;
    Result->Range.Start = 0;
    Result->Range.End = 0;
    Result->Next = 0;

    return(Result);
}

static json_token_list *
AppendList(json_token_list *List, json_token_type Type, json_token_range Range)
{
    json_token_list *Result = malloc(sizeof(json_token_list));
    Result->Type = Type;
    Result->Range = Range;
    Result->Next = 0;

    List->Next = Result;

    return(Result);
}

static json_token_list*
AppendListWithTokenType(json_token_list *List, json_token_type Type)
{
    json_token_range Range = {0,0};
    json_token_list *Result = AppendList(List, Type, Range);

    return(Result);
}

static void
ParserError(json_parser *Parser, char *Message)
{
    PrintError(Message);
    Parser->State = json_parser_state_Error;
}

static void
ParseSpace(buffer *Buffer, json_parser *Parser)
{
    while(CharIsSpace(Buffer->Data[Parser->Index]) && (Parser->Index < Buffer->Size))
    {
        Parser->Index++;
    }
}

static void
ParseKeyword(buffer *Buffer, json_parser *Parser, char *Keyword)
{
    u32 KeywordIndex = 0;
    b32 Matched = True;
    u32 DEBUG_Iterations = 0;

    for(;;)
    {
        if(DEBUG_Iterations++ > 32)
        {
            PrintError("ParseKeyword max iter\n");
            break;
        }
        else if(CharIsNullChar(Keyword[KeywordIndex]))
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

static json_token_range
ParseString(buffer *Buffer, json_parser *Parser)
{
    json_token_range Result;
    Result.Start = Parser->Index;
    Result.End = Result.Start;
    u32 DEBUG_Iterations = 0;

    if(GetChar(Buffer, Parser) == '"')
    {
        Parser->Index++;

        for(;;)
        {
            DEBUG_Iterations++;
            if(Parser->Index >= Buffer->Size)
            {
                break;
            }
            else if(DEBUG_Iterations > 1000)
            {
                PrintError("ParseString max iter");
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

static json_token_type
ParseBoolean(buffer *Buffer, json_parser *Parser)
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
            Result = json_token_type_True;
        }
    } break;
    default:
    {
        ParserError(Parser, "parsing boolean");
    }
    }

    return(Result);
}

static json_token_range
ParseNumber(buffer *Buffer, json_parser *Parser)
{
    json_token_range Result = {Parser->Index, Parser->Index};
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

json_token_list *
ParseJson(buffer *Buffer)
{
    json_token_list *Result = CreateTokenListHead();
    json_token_list *Last = Result;
    json_parser Parser = { 0, json_parser_state_Running };
    int DEBUG_Iterations = 0;
    int DEBUG_MaxIterations = 1000;

    while((Parser.State == json_parser_state_Running) &&
          (DEBUG_Iterations++ < DEBUG_MaxIterations))
    {
        ParseSpace(Buffer, &Parser);

        if(Parser.Index >= Buffer->Size)
        {
            // end of buffer
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
            json_token_range Range = ParseString(Buffer, &Parser);

            if(Range.End == Range.Start)
            {
                ParserError(&Parser, "Invalid string token range");
            }

            Last = AppendList(Last, json_token_type_String, Range);
            Parser.Index++;
        } break;
        default:
        {
            // TODO: break up the logic for numbers and booleans
            json_token_type TokenType = json_token_type_Empty;
            json_token_range TokenRange = {0,0};

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

    if(DEBUG_Iterations++ >= DEBUG_MaxIterations)
    {
        PrintError("Max Iterations");
    }

    return(Result);
}
