#include <stdio.h>

#include "parser.h"

#define GetParserState(Parser) (Parser.State[Parser.StatePosition])
#define SetParserState(Parser, NewState) (Parser.State[Parser.StatePosition] = NewState)

#define ArrayItemSize(Array) (sizeof(Array[0]))
#define ArrayCount(Array) (sizeof(Array) / ArrayItemSize(Array))

#define CharIsUpperCase(Char) (Char >= 'A' && Char <= 'Z')
#define CharIsLowerCase(Char) (Char >= 'a' && Char <= 'z')
#define CharIsDigit(Char) (Char >= '0' && Char <= '9')
#define CharIsAlphaNum(Char) (CharIsUpperCase(Char) || CharIsLowerCase(Char) || CharIsDigit(Char))
#define CharIsSpace(Char) ((Char == ' ') || (Char == '\n') || (Char == '\r') || (Char == '\t'))

void
Error(const char *Message)
{
    printf("Error: %s\n", Message);
    exit(1);
}

static parser
CreateParser()
{
    parser Result = {};
    Result.At = 0;
    Result.State[0] = ParserStateTopLevel;
    Result.StatePosition = 0;

    return Result;
}

static token_list *
CreateToken(u32 Type, buffer *Buffer)
{
    token_list *Result = (token_list *)malloc(sizeof(token_list));
    Result->Type = Type;
    Result->Text = Buffer;
    Result->Next = NULL;

    return Result;
}

static token_list *
PushToken(token_list *TokenList, token_type Type, buffer *Text)
{
    TokenList->Next = CreateToken(Type, Text);

    return TokenList->Next;
}

static b32
MatchText(buffer Buffer, parser *Parser, const char *Text)
{
    b32 Result = 1;
    size Offset = 0;

    for(;;)
    {
        if(Text[Offset] == '\0')
        {
            break;
        }
        else if((Offset >= Buffer.Size) ||
                (Buffer.Data[Parser->At + Offset] != Text[Offset]))
        {
            Result = 0;
            break;
        }
        else
        {
            Offset++;
        }
    }

    if(Result)
    {
        Parser->At += Offset;
    }

    return Result;
}

void
SkipSpace(buffer Buffer, parser *Parser)
{
    while(Parser->At >= Buffer.Size || CharIsSpace(Buffer.Data[Parser->At]))
    {
        Parser->At++;
    }
}

token_range
CreateTokenRange(size Begin, size End)
{
    token_range Result;
    Result.Begin = Begin;
    Result.End = End;

    return Result;
}

token_range
ParseTitleString(buffer Buffer, parser *Parser)
{
    token_range Result = CreateTokenRange(Parser->At, Parser->At);
    b32 Success = True;
    size Offset = 0;

    for(;;)
    {
        if(Offset >= Buffer.Size)
        {
            break;
        }
        else
        {
            u8 Char = Buffer.Data[Parser->At + Offset];

            if(Offset == 0 && !CharIsUpperCase(Char))
            {
                Success = False;
                break;
            }
            else if(!CharIsAlphaNum(Char))
            {
                break;
            }

            Offset++;
        }
    }

    if(Success == True)
    {
        Parser->At += Offset;
        Result.End = Result.Begin + Offset;
    }

    return Result;
}

void
PushParserState(parser *Parser, parser_state ParserState)
{
    if(Parser->StatePosition >= ArrayCount(Parser->State))
    {
        Error("Calling PushParserState with a full state stack");
    }
    else
    {
        Parser->StatePosition++;
        Parser->State[Parser->StatePosition] = ParserState;
    }
}

parser_state
PopParserState(parser *Parser)
{
    parser_state Result = ParserStateEnd;

    if(Parser->StatePosition < 0)
    {
        Error("PopParserState called with empty stack");
    }
    else
    {
        Result = Parser->State[Parser->StatePosition];
        Parser->StatePosition--;
    }

    return Result;
}

token_list *
Parse(buffer Buffer)
{
    token_list *Result = CreateToken(TokenTypeStreamBegin,
                                     BufferFromNullTerminatedString(""));
    token_list *CurrentToken = Result;
    parser Parser = CreateParser();
    const u32 MaxIterCount = 9999;
    u32 Iter = 0;

    while(GetParserState(Parser) != ParserStateEnd && (Iter++ < MaxIterCount))
    {
        if(Parser.At >= Buffer.Size)
        {
            SetParserState(Parser, ParserStateEnd);
        }
        else
        {
            u8 CurrentChar = Buffer.Data[Parser.At];

            switch(GetParserState(Parser))
            {
                case ParserStateTopLevel:
                {
                    switch(CurrentChar)
                    {
                        case 'b':
                        {
                            SetParserState(Parser, ParserStateBind);
                        } break;
                        case 'm':
                        {
                            SetParserState(Parser, ParserStateMatch);
                        } break;
                        default:
                        {
                            CurrentToken = PushToken(CurrentToken,
                                                     TokenTypeError,
                                                     BufferFromNullTerminatedString("Expected top-level keyword: bind, match"));
                            SetParserState(Parser, ParserStateEnd);
                        }
                    }
                } break;
                case ParserStateBind:
                {
                    b32 MatchTextResult = MatchText(Buffer, &Parser, "bind");

                    if(MatchTextResult)
                    {
                        CurrentToken = PushToken(CurrentToken,
                                                 TokenTypeKeywordBind,
                                                 BufferFromNullTerminatedString("bind"));
                        SkipSpace(Buffer, &Parser);
                        SetParserState(Parser, ParserStateBindName);
                    }
                    else
                    {
                        CurrentToken = PushToken(CurrentToken,
                                                 TokenTypeError,
                                                 BufferFromNullTerminatedString("Expected keyword: bind"));
                        SetParserState(Parser, ParserStateEnd);
                    }
                } break;
                case ParserStateVariable:
                {
                    token_range TitleStringRange = ParseTitleString(Buffer, &Parser);
                    size RangeSize = TitleStringRange.End - TitleStringRange.Begin;

                    if(RangeSize > 0)
                    {
                        buffer *SubBuffer = GetBufferSubRegion(&Buffer, TitleStringRange.Begin, TitleStringRange.End);

                        CurrentToken = PushToken(CurrentToken,
                                                 TokenTypeVariable,
                                                 SubBuffer);
                        SkipSpace(Buffer, &Parser);
                        PopParserState(&Parser);
                    }
                    else
                    {
                        CurrentToken = PushToken(CurrentToken,
                                                 TokenTypeError,
                                                 BufferFromNullTerminatedString("Error parsing variable"));
                        SetParserState(Parser, ParserStateEnd);
                    }
                } break;
                case ParserStateBindName:
                {
                    token_range TitleStringRange = ParseTitleString(Buffer, &Parser);

                    if((TitleStringRange.End - TitleStringRange.Begin) > 0)
                    {
                        buffer *SubBuffer = GetBufferSubRegion(&Buffer, TitleStringRange.Begin, TitleStringRange.End);

                        CurrentToken = PushToken(CurrentToken,
                                                 TokenTypeVariable,
                                                 SubBuffer);
                        SkipSpace(Buffer, &Parser);
                        SetParserState(Parser, ParserStateBindBlockOpen);
                    }
                    else
                    {
                        CurrentToken = PushToken(CurrentToken,
                                                 TokenTypeError,
                                                 BufferFromNullTerminatedString("Error parsing bind name"));
                        SetParserState(Parser, ParserStateEnd);
                    }
                } break;
                case ParserStateBindBlockOpen:
                {
                    if(Buffer.Data[Parser.At] == '{')
                    {
                        Parser.At++;
                        SkipSpace(Buffer, &Parser);
                         CurrentToken = PushToken(CurrentToken,
                                                  TokenTypeCurlyOpen,
                                                  BufferFromNullTerminatedString("{"));
                        SetParserState(Parser, ParserStateBindStatements);
                    }
                    else
                    {
                        CurrentToken = PushToken(CurrentToken,
                                                 TokenTypeError,
                                                 BufferFromNullTerminatedString("Expected block open: {"));
                        SetParserState(Parser, ParserStateEnd);
                    }
                } break;
                case ParserStateBindStatements:
                {
                    if(Buffer.Data[Parser.At] == '}')
                    {
                        SetParserState(Parser, ParserStateBindBlockClose);
                    }
                    else
                    {
                        SetParserState(Parser, ParserStateBindStatementOperator);
                        PushParserState(&Parser, ParserStateVariable);
                    }
                } break;
                case ParserStateBindStatementOperator:
                {
                    SkipSpace(Buffer, &Parser);
                    b32 MatchTextResult = MatchText(Buffer, &Parser, ":");

                    if (MatchTextResult)
                    {
                        CurrentToken = PushToken(CurrentToken,
                                                 TokenTypeOperatorEqual,
                                                 BufferFromNullTerminatedString(":"));
                        SkipSpace(Buffer, &Parser);
                        SetParserState(Parser, ParserStateBindExpression);
                    }
                    else
                    {
                        CurrentToken = PushToken(CurrentToken,
                                                 TokenTypeError,
                                                 BufferFromNullTerminatedString("Expected operator: :"));
                        SetParserState(Parser, ParserStateEnd);
                    }
                } break;
                case ParserStateBindExpression:
                {
                    SetParserState(Parser, ParserStateEnd);
                } break;
                case ParserStateBindBlockClose:
                {
                    CurrentToken = PushToken(CurrentToken,
                                             TokenTypeCurlyClose,
                                             BufferFromNullTerminatedString("}"));
                    SetParserState(Parser, ParserStateTopLevel);
                } break;
                case ParserStateEnd:
                {
                    CurrentToken = PushToken(CurrentToken,
                                             TokenTypeStreamEnd,
                                             BufferFromNullTerminatedString(""));
                } break;
            }
        }
    }

    return Result;
}
