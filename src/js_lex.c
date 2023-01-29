#define in_bounds(buffer, i) ((*i) < (buffer)->Size)
#define get_char(buffer, i) ((buffer)->Data[(*i)])
#define next(i) ((*(i))++)

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

static token ScanString(buffer *Source, size *I)
{
    token Result;
    Result.Kind = token_kind_None;
    u8 MatchQuote = get_char(Source, I);
    next(I);
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

static token ParseToken(buffer *Source, size *I)
{
    token Token;
    b32 Running = 1;
    EatSpace(Source, I);
    while(Running && in_bounds(Source, I))
    {
        switch(get_char(Source, I))
        {
        case '{': case '}': case '[': case ']': case '(': case ')':
        case ';': case ':':  case ',': case '.':
            Token.Kind = get_char(Source, I);
            ++(*I);
            Running = 0;
            break;
        case '"': case '\'': case '`':
            Token = ScanString(Source, I);
            Running = 0;
            break;
        default:
            Token.Kind = token_kind_None;
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
    buffer *Source = ReadFileIntoBuffer("./test/test.js");
    u32 I;
    token *Tokens = LexJs(Source);
    printf("Tokens\n");
    for(I = 0; I < vec_len(Tokens); ++I)
    {
        if(Tokens[I].Kind < 128)
        {
            printf("%c ", Tokens[I].Kind);
        }
        else
        {
            printf("%d ", Tokens[I].Kind);
        }
    }
    printf("\n");
    return Result;
}
