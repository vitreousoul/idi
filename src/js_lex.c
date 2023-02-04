#define in_bounds(buffer, i) ((*i) < (buffer)->Size)
#define get_char(buffer, i) ((buffer)->Data[(*i)])
#define char_is_digit(c) ((c) >= '0' && (c) <= '9')
#define char_is_alpha_lower(c) ((c) >= 'a' && (c) <= 'z')
#define char_is_alpha_upper(c) ((c) >= 'A' && (c) <= 'Z')
#define char_is_alpha(c) (char_is_alpha_lower(c) || char_is_alpha_upper(c))
#define char_is_alpha_num(c) (char_is_alpha(c) || char_is_digit(c))
#define next(i) ((*(i))++)
#define next_index(i) ((*(i))+1)

static void EatSpace(buffer *Source, size *I)
{
    while(in_bounds(Source, I))
    {
        switch(get_char(Source, I))
        {
        case ' ': case '\n': case '\r': case '\t':
            ++*I;
            break;
        default:
            goto end;
        }
    }
end:;
}

static token EmptyToken()
{
    token Result;
    Result.Kind = token_kind_None;
    return Result;
}

static u8 Peek(buffer *Source, size *I)
{
    if(in_bounds(Source, I))
    {
        size NextI = *I + 1;
        return get_char(Source, &NextI);
    }
    else
    {
        return 0;
    }
}

static token ScanString(buffer *Source, size *I)
{
    token Result;
    Result.Kind = token_kind_None;
    u8 MatchQuote = get_char(Source, I);
    next(I);
    Result.Value.String.Start = *I;
    size StartI = *I;
    while(in_bounds(Source, I))
    {
        u8 Char = get_char(Source, I);
        if(Char == MatchQuote)
        {
            Result.Kind = token_kind_String;
            Result.Value.String.Start = StartI;
            Result.Value.String.End = *I;
            next(I);
            break;
        }
        else if(Char == '\\')
        {
            next(I);
        }
        next(I);
    }
    return Result;
}

static token ScanDigit(buffer *Source, size *I)
{
    token Result;
    Result.Kind = token_kind_Integer;
    Result.Value.String.Start = *I;
    while(in_bounds(Source, I))
    {
        u8 Char = get_char(Source, I);
        if(Char == '.')
        {
            if(Result.Kind == token_kind_Float)
            {
                Result.Kind = token_kind_None;
                break;
            }
            else
            {
                Result.Kind = token_kind_Float;
                next(I);
            }
        }
        else if(char_is_digit(Char))
        {
            next(I);
        }
        else
        {
            break;
        }
    }
    return Result;
}

static token ScanIdentifier(buffer *Source, size *I)
{
    token Result;
    Result.Kind = token_kind_Identifier;
    Result.Value.String.Start = *I;
    while(in_bounds(Source, I) && char_is_alpha_num(get_char(Source, I)))
    {
        next(I);
    }
    Result.Value.String.End = (*I);
    return Result;
}

static token ParseToken(buffer *Source, size *I)
{
    token Token = EmptyToken();
    b32 Running = 1;
    EatSpace(Source, I);
    while(Running && in_bounds(Source, I))
    {
        printf("char %c\n", get_char(Source, I));
        switch(get_char(Source, I))
        {
        case '{': case '}': case '[': case ']': case '(': case ')':
        case ';': case ':':  case ',': case '=':
        singlechar:
            Token.Kind = get_char(Source, I);
            next(I);
            Running = 0;
            break;
        case '"': case '\'': case '`':
            Token = ScanString(Source, I);
            Running = 0;
            break;
        case '.':
            if(char_is_digit(Peek(Source, I)))
            {
                Token = ScanDigit(Source, I);
            }
            else
            {
                goto singlechar;
            }
            Running = 0;
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            Token = ScanDigit(Source, I);
            Running = 0;
            break;
        case 'a': case 'b': case 'c': case 'd': case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
            Token = ScanIdentifier(Source, I);
            Running = 0;
            break;
        default:
            Running = 0;
            break;
        }
    }
    return Token;
}

static token *LexJs(buffer *Source)
{
    token *Result = 0;
    size I = 0;
    while(in_bounds(Source, &I))
    {
        token Token = ParseToken(Source, &I);
        if(Token.Kind != token_kind_None)
        {
            printf("pushing token %d %lu\n", Token.Kind, I);
            vec_push(Result, Token);
        }
        else
        {
            break;
        }
    }
    return Result;
}

u32 TestJsLex()
{
    printf("\nTestJsLex:\n");
    u32 Result = 0;
    buffer *Source = ReadFileIntoBuffer("../test/test.js");
    u32 I;
    token *Tokens = LexJs(Source);
    printf("Tokens %lu\n", vec_len(Tokens));
    for(I = 0; I < vec_len(Tokens); ++I)
    {
        if(Tokens[I].Kind < 128)
        {
            printf("%c ", Tokens[I].Kind);
        }
        else if(Tokens[I].Kind == 0)
        {
            printf("NONE ");
        }
        else
        {
            token Token = Tokens[I];
            u32 StringLength = (u32)Token.Value.String.End - Token.Value.String.Start;
            u8 *SourceStart = &Source->Data[Token.Value.String.Start];
            switch(Token.Kind)
            {
            case token_kind_String: printf("<String %.*s>", StringLength, SourceStart); break;
            case token_kind_Integer: printf("<Integer %.*s>", StringLength, SourceStart); break;
            case token_kind_Float: printf("<Float %.*s>", StringLength, SourceStart); break;
            case token_kind_Identifier: printf("<Identifier %.*s>", StringLength, SourceStart); break;
            default: printf("<no token>"); break;
            }
        }
    }
    printf("\n");
    return Result;
}
