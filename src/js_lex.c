#define in_bounds(lexer) ((lexer)->I < (lexer)->Source.Size)
#define get_char(lexer) ((lexer)->Source.Data[(lexer)->I])
#define char_is_digit(c) ((c) >= '0' && (c) <= '9')
#define char_is_alpha_lower(c) ((c) >= 'a' && (c) <= 'z')
#define char_is_alpha_upper(c) ((c) >= 'A' && (c) <= 'Z')
#define char_is_alpha(c) (char_is_alpha_lower(c) || char_is_alpha_upper(c))
#define char_is_alpha_num(c) (char_is_alpha(c) || char_is_digit(c))
#define next(i) ((*(i))++)
#define next_index(i) ((*(i))+1)

static void EatSpace(lexer *Lexer)
{
    while(in_bounds(Lexer))
    {
        switch(get_char(Lexer))
        {
        case ' ': case '\n': case '\r': case '\t':
            Lexer->I++;
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

static u8 Peek(lexer *Lexer)
{
    if(in_bounds(Lexer))
    {
        return get_char(Lexer);
    }
    else
    {
        return 0;
    }
}

static token ScanString(lexer *Lexer)
{
    token Result;
    Result.Kind = token_kind_None;
    u8 MatchQuote = get_char(Lexer);
    ++Lexer->I;
    Result.Value.String.Start = Lexer->I;
    size StartI = Lexer->I;
    while(in_bounds(Lexer))
    {
        u8 Char = get_char(Lexer);
        if(Char == MatchQuote)
        {
            Result.Kind = token_kind_String;
            Result.Value.String.Start = StartI;
            Result.Value.String.End = Lexer->I;
            ++Lexer->I;
            break;
        }
        else if(Char == '\\')
        {
            ++Lexer->I;
        }
        ++Lexer->I;
    }
    return Result;
}

static token ScanDigit(lexer *Lexer)
{
    token Result;
    Result.Kind = token_kind_Integer;
    Result.Value.String.Start = Lexer->I;
    while(in_bounds(Lexer))
    {
        u8 Char = get_char(Lexer);
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
                ++Lexer->I;
            }
        }
        else if(char_is_digit(Char))
        {
            ++Lexer->I;
        }
        else
        {
            break;
        }
    }
    Result.Value.String.End = Lexer->I;
    return Result;
}

static token ScanIdentifier(lexer *Lexer)
{
    token Result;
    Result.Kind = token_kind_Identifier;
    Result.Value.String.Start = Lexer->I;
    while(in_bounds(Lexer) && char_is_alpha_num(get_char(Lexer)))
    {
        ++Lexer->I;
    }
    Result.Value.String.End = (Lexer->I);
    return Result;
}

static token ParseToken(lexer *Lexer)
{
    token Token = EmptyToken();
    b32 Running = 1;
    EatSpace(Lexer);
    while(Running && in_bounds(Lexer))
    {
        switch(get_char(Lexer))
        {
        case '{': case '}': case '[': case ']': case '(': case ')':
        case ';': case ':':  case ',': case '=': case '*':
        singlechar:
                Token.Kind = get_char(Lexer);
                ++Lexer->I;
                Running = 0;
                break;
        case '"': case '\'': case '`':
            Token = ScanString(Lexer);
            Running = 0;
            break;
        case '.':
            if(char_is_digit(Peek(Lexer)))
            {
                Token = ScanDigit(Lexer);
            }
            else
            {
                goto singlechar;
            }
            Running = 0;
            break;
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            Token = ScanDigit(Lexer);
            Running = 0;
            break;
        case 'a': case 'b': case 'c': case 'd': case 'e':
        case 'f': case 'g': case 'h': case 'i': case 'j':
        case 'k': case 'l': case 'm': case 'n': case 'o':
        case 'p': case 'q': case 'r': case 's': case 't':
        case 'u': case 'v': case 'w': case 'x': case 'y': case 'z':
        case 'A': case 'B': case 'C': case 'D': case 'E':
        case 'F': case 'G': case 'H': case 'I': case 'J':
        case 'K': case 'L': case 'M': case 'N': case 'O':
        case 'P': case 'Q': case 'R': case 'S': case 'T':
        case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z':
            Token = ScanIdentifier(Lexer);
            Running = 0;
            break;
        default:
            Running = 0;
            break;
        }
    }
    return Token;
}

static token *LexJs(lexer *Lexer)
{
    token *Result = 0;
    while(in_bounds(Lexer))
    {
        token Token = ParseToken(Lexer);
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
    size I, J;
    u32 Indent = 0;
    char *FilePath = "../test/test.js";
    lexer Lexer = {*ReadFileIntoBuffer(FilePath),0};
    token *Tokens = LexJs(&Lexer);
    printf("Tokens %lu\n", vec_len(Tokens));
    for(I = 0; I < vec_len(Tokens); ++I)
    {
        token Token = Tokens[I];
        // indentation
        if(Token.Kind == '{' || Token.Kind == '(' || Token.Kind == '[') {
            for(J = 0; J < Indent; J++) printf(" ");
            Indent += 4;
        } else if(Token.Kind == '}' || Token.Kind == ')' || Token.Kind == ']') {
            Indent -= 4;
            for(J = 0; J < Indent; J++) printf(" ");
        } else {
            for(J = 0; J < Indent; J++) printf(" ");
        }
        // print token
        if(Token.Kind < 128)
        {
            printf("(char '%c')", Token.Kind);
        }
        else if(Tokens[I].Kind == 0)
        {
            printf("NONE ");
        }
        else
        {
            token Token = Tokens[I];
            u32 StringLength = (u32)Token.Value.String.End - Token.Value.String.Start;
            u8 *SourceStart = &Lexer.Source.Data[Token.Value.String.Start];
            switch(Token.Kind)
            {
            case token_kind_String:
                printf("(str \"%.*s\")", StringLength, SourceStart);
                break;
            case token_kind_Integer:
                printf("(int %.*s)", StringLength, SourceStart);
                break;
            case token_kind_Float:
                printf("(float %.*s)", StringLength, SourceStart);
                break;
            case token_kind_Identifier:
                printf("(ident %.*s)", StringLength, SourceStart);
                break;
            default: printf("()"); break;
            }
        }
        printf("\n");
    }
    printf("\n");
    return Result;
}
