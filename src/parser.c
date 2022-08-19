#include <stdio.h>

#include "parser.h"

enum parser_state {
ParserStateTopLevel,
ParserStateBind,
ParserStateBindName,
ParserStateMatch,
ParserStateEnd
};
typedef enum parser_state parser_state;

#define GetParserState(Parser) (Parser.State[Parser.StatePosition])
#define SetParserState(Parser, NewState) (Parser.State[Parser.StatePosition] = NewState)

static parser
CreateParser()
{
    parser Result = {};
    Result.At = 0;
    Result.State[0] = ParserStateTopLevel;
    Result.StatePosition = 0;

    return Result;
}

b32
MatchText(buffer Buffer, parser *Parser, const char *Text)
{
    b32 Result = 1;
    size Offset = 0;

    for(;;)
    {
        if (Text[Offset] == '\0')
        {
            break;
        }
        else if ((Offset >= Buffer.Size) ||
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

    if (Result)
    {
        Parser->At += Offset;
    }

    return Result;
}

void
SkipSpace(buffer Buffer, parser *Parser)
{
    while(Parser->At >= Buffer.Size || Buffer.Data[Parser->At] == ' ')
    {
        Parser->At++;
    }
}

token_list *
Parse(buffer Buffer)
{
    token_list *Result = malloc(sizeof(token_list));
    parser Parser = CreateParser();
    const u32 MaxIterCount = 9999;
    u32 Iter = 0;

    while(GetParserState(Parser) != ParserStateEnd && (Iter++ < MaxIterCount))
    {
        if (Parser.At >= Buffer.Size)
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
                            // TODO: push error token
                            SetParserState(Parser, ParserStateEnd);
                        }
                    }
                } break;
                case ParserStateBind:
                {
                    b32 MatchTextResult = MatchText(Buffer, &Parser, "bind");

                    if (MatchTextResult)
                    {
                        SkipSpace(Buffer, &Parser);
                        SetParserState(Parser, ParserStateBindName);
                    }
                    else
                    {
                        token_list *NextToken = (token_list *)malloc(sizeof(token_list));
                        NextToken->Type = 42;
                        Result->Next = NextToken;
                        SetParserState(Parser, ParserStateEnd);
                    }
                } break;
                case ParserStateBindName:
                {
                    SetParserState(Parser, ParserStateEnd);
                    // TODO: implement
                } break;
                case ParserStateEnd:
                {
                } break;
            }
        }
    }

    return Result;
}
