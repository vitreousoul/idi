#define in_bounds(lexer) ((lexer)->I < (lexer)->Source.Size)
#define get_char(lexer) ((lexer)->Source.Data[(lexer)->I])
#define char_is_digit(c) ((c) >= '0' && (c) <= '9')
#define char_is_alpha_lower(c) ((c) >= 'a' && (c) <= 'z')
#define char_is_alpha_upper(c) ((c) >= 'A' && (c) <= 'Z')
#define char_is_alpha(c) (char_is_alpha_lower(c) || char_is_alpha_upper(c))
#define char_is_alpha_num(c) (char_is_alpha(c) || char_is_digit(c))
#define char_is_identifier(c) (char_is_alpha_num(c) || (c) == '_')

#define IDENTIFIER_TABLE_COUNT 1 << 10

#define STRING_BUFFER_SIZE 1 << 14
char GLOBAL_STRING_BUFFER_DATA[STRING_BUFFER_SIZE];
string_buffer GLOBAL_STRING_BUFFER = {STRING_BUFFER_SIZE,0,GLOBAL_STRING_BUFFER_DATA};

KeywordKind KEYWORD_KINDS[] = {
    {token_kind_Import, "import"},
    {token_kind_Export, "export"},
    {token_kind_From, "from"},
    {token_kind_Var, "var"},
    {token_kind_Let, "let"},
    {token_kind_Const, "const"},
    {token_kind_Function, "function"},
    {token_kind_Return, "return"},
    {token_kind_As, "as"},
    {token_kind_Default, "default"},
};

static u8 Peek(lexer *Lexer)
{
    if(in_bounds(Lexer))
    {
        return Lexer->Source.Data[Lexer->I + 1];
    }
    else
    {
        return 0;
    }
}

static hash_table InitIdentifierTable()
{
    u32 I;
    hash_table IdentifierTable = CreateHashTable(IDENTIFIER_TABLE_COUNT);
    for(I = 0; I < ArrayCount(KEYWORD_KINDS); ++I)
    {
        HashTableSet(&IdentifierTable, KEYWORD_KINDS[I].String, KEYWORD_KINDS[I].TokenKind);
    }
    return IdentifierTable;
}

static char *StringFromRange(lexer *Lexer, u32 Start, u32 End)
{
    u32 StringLength = 1 + End - Start;
    char *Result = malloc(sizeof(char) * StringLength); // TODO: use string buffer here instead
    memcpy(Result, &Lexer->Source.Data[Lexer->I], StringLength);
    Result[StringLength - 1] = 0; // null terminate
    return Result;
}

static void EatSpace(lexer *Lexer)
{
    while(in_bounds(Lexer))
    {
        switch(get_char(Lexer))
        {
        case ' ': case '\n': case '\r': case '\t':
            Lexer->I++;
            break;
        case '/':
        {
            u8 NextChar = Peek(Lexer);
            if (NextChar == '/')
            {
                Lexer->I += 2;
                b32 Running = 1;
                while(Running && in_bounds(Lexer))
                {
                    u8 Char = get_char(Lexer);
                    if(Char == '\n')
                    {
                        ++Lexer->I;
                        Running = 0;
                    }
                    else if(Char == '\r' && Peek(Lexer) == '\n')
                    {
                        Lexer->I += 2;
                        Running = 0;
                    }
                    else
                    {
                        ++Lexer->I;
                    }
                }
            }
            else if (NextChar == '*')
            {
                Lexer->I += 2;
                b32 Running = 1;
                while(Running && in_bounds(Lexer))
                {
                    if(get_char(Lexer) == '*' && Peek(Lexer) == '/')
                    {
                        Lexer->I += 2;
                        Running = 0;
                    }
                    else
                    {
                        ++Lexer->I;
                    }
                }
            }
            else
            {
                goto end;
            }
        } break;
        default:
            goto end;
        }
    }
end:;
}

static token EmptyToken()
{
    token Result;
    memset(&Result, 0, sizeof(token));
    return Result;
}

static token ArrowToken()
{
    token Result;
    Result.Kind = token_kind_Arrow;
    return Result;
}

static token ScanString(lexer *Lexer)
{
    token Result;
    u32 Start, End;
    Result.Kind = token_kind_None;
    u8 MatchQuote = get_char(Lexer);
    ++Lexer->I; // increment past the quote character
    Start = Lexer->I;
    while(in_bounds(Lexer))
    {
        u8 Char = get_char(Lexer);
        if(Char == MatchQuote)
        {
            Result.Kind = token_kind_String;
            break;
        }
        else if(Char == '\\')
        {
            ++Lexer->I;
        }
        ++Lexer->I;
    }
    End = Lexer->I;
    ++Lexer->I;
    Result.String = StringFromRange(Lexer, Start, End);
    return Result;
}

static token ScanDigit(lexer *Lexer)
{
    token Result;
    u32 Start = 0, End = 0;
    Result.Kind = token_kind_Integer;
    Start = Lexer->I;
    b32 Exponential = 0;
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
        else if(Char == 'e')
        {
            if(Exponential)
            {
                Result.Kind = token_kind_None;
                break;
            }
            else if(Peek(Lexer) == '-')
            {
                ++Lexer->I;
            }
            ++Lexer->I;
            Exponential = 1;
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
    End = Lexer->I;
    Result.String = StringFromRange(Lexer, Start, End);
    return Result;
}

static token ScanEquals(lexer *Lexer)
{
    token Token;
    ++Lexer->I;
    u8 NextChar = Peek(Lexer);
    if(NextChar == '=')
    {
        Token.Kind = token_kind_TripleEquals;
        ++Lexer->I;
    }
    else
    {
        Token.Kind = token_kind_DoubleEquals;
    }
    ++Lexer->I;
    return Token;
}

static token ScanIdentifier(lexer *Lexer, hash_table IdentifierTable)
{
    token Result;
    token_kind KeywordKind;
    u32 StringLength, Start = 0, End = 0;
    Result.Kind = token_kind_Identifier;
    Start = Lexer->I;
    while(in_bounds(Lexer) && char_is_identifier(get_char(Lexer)))
    {
        ++Lexer->I;
    }
    End = (Lexer->I);
    StringLength = 1 + End - Start;
    char String[StringLength]; // TODO: get this string off the stack
    memcpy(String, &Lexer->Source.Data[Lexer->I], StringLength);
    String[StringLength - 1] = 0; // null terminate
    if((KeywordKind = HashTableGet(&IdentifierTable, String)))
    {
        Result.Kind = KeywordKind;
    }
    return Result;
}

static token ParseToken(lexer *Lexer, hash_table IdentifierTable)
{
    token Token = EmptyToken();
    b32 Running = 1;
    EatSpace(Lexer);
    while(Running && in_bounds(Lexer))
    {
        switch(get_char(Lexer))
        {
        case '{': case '}': case '[': case ']': case '(': case ')':
        case ';': case ':':  case ',': case '*': case '?':
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
        case '=':
        {
            u8 NextChar = Peek(Lexer);
            if(NextChar == '=')
            {
                Token = ScanEquals(Lexer);
            }
            else if(NextChar == '>')
            {
                Token = ArrowToken();
                Lexer->I += 2;
            }
            else
            {
                goto singlechar;
            }
        } break;
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
        case '_':
            Token = ScanIdentifier(Lexer, IdentifierTable);
            Running = 0;
            break;
        default:
            Running = 0;
            break;
        }
    }
    return Token;
}

token *LexJs(lexer *Lexer)
{
    token *Result = 0;
    hash_table IdentifierTable = InitIdentifierTable();
    u32 ReservedKeywordCount = 0;
    while(in_bounds(Lexer))
    {
        token Token = ParseToken(Lexer, IdentifierTable);
        if(Token.Kind != token_kind_None)
        {
            if(Token.Kind == token_kind_Identifier)
            {
                identifier_kind KeywordKind = HashTableGet(&IdentifierTable, Token.String);
                if (KeywordKind == identifier_kind_Keyword) ++ReservedKeywordCount;
            }
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
            switch(Token.Kind)
            {
            case token_kind_String:
                printf("(str \"%s\")", Token.String);
                break;
            case token_kind_Integer:
                printf("(int %s)", Token.String);
                break;
            case token_kind_Float:
                printf("(float %s)", Token.String);
                break;
            case token_kind_Identifier:
                printf("(ident %s)", Token.String);
                break;
            case token_kind_DoubleEquals:
                printf("(double-equals)");
                break;
            case token_kind_TripleEquals:
                printf("(triple-equals)");
                break;
            case token_kind_Arrow:
                printf("(arrow)");
                break;
            default: printf("(%d)", Token.Kind); break;
            }
        }
        printf("\n");
    }
    printf("\n");
    return Result;
}
