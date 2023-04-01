// ==========
//   Grammar
// ==========

// Import = /import/ Body? Path /;/
// Path = String
// Body = Default | Star | Pick
// Star = /\*/ /as/ Identifier /from/
// Default = Identifier DefaultEx
// DefaultEx = /from/ | BodyEx
// BodyEx = /,/ StarOrPick
// StarOrPick = Star | Pick
// Pick = /\{/ PickParams
// PickParams = /\}/ | PickItems
// PickItems = PickItem PickRest
// PickRest = /}/ |  PickNext* PickEnd
// PickNext = /,/ PickItem?
// PickEnd = /,}/ | /}/
// PickNext = /,/ PickItem
// PickItem = PickName PickAlias?
// PickName = /default/ | String | Identifier
// PickAlias = /as/ Identifier

// Function = Arrow | Func
// Arrow = FunctionParams /=>/ FunctionArrowBody
// Func = /function/ FunctionParams FunctionBlock
// FunctionBody = FunctionBlock | Expr
// FunctionBlock = /{/ Statement* /}/
// FunctionParams = /(/ FunctionParam* /)/
// FunctionParam = Identifier FunctionRest* /,/?
// FunctionRest = /,/ Identifier

// Assignment = AssignmentVar | AssignmentLet | AssignmentConst
// AssignmentVar = /var/ AssignmentBody /;/
// AssignmentLet = /let/ AssignmentBody /;/
// AssignmentConst = /const/ AssignmentBody /;/
// AssignmentBody = Identifier /=/ Expr

// Expr = ExprParen | ExprValue
// ExprParen = /(/ Expr /)/
// ExprValue = ArithmeticExpr | Function | FunctionCall

// ArithmeticExpr = ArithmeticExpr /+/ Term | ArithmeticExpr /-/ Term | Term
// Term = Term /*/ Factor | Term /// Factor | Factor
// Factor = Digit | /(/ ArithmeticExpr /)/

#define PREVIOUS_TOKEN(Parser) ((Parser)->Tokens[(Parser)->I-1])
#define CURRENT_TOKEN(Parser) (HAS_TOKENS(Parser) ? (Parser)->Tokens[(Parser)->I] : EmptyToken())
#define HAS_TOKENS(Parser) ((Parser)->I < (Parser)->TokenCount)

#define EMIT_BUFFER_SIZE 2048
char EmitBuffer[EMIT_BUFFER_SIZE];

static void ParseError()
{
    printf("[ ERROR ] ParseError\n");
    exit(1);
}

static void NextToken(js_parser *Parser)
{
    if(HAS_TOKENS(Parser))
    {
        ++Parser->I;
    }
    else
    {
        printf("Out of tokens\n");
        ParseError();
    }
}

static void ExpectToken(js_parser *Parser, token_kind TokenKind)
{
    token_kind CurrentTokenKind = CURRENT_TOKEN(Parser).Kind;
    if(!HAS_TOKENS(Parser))
    {
        printf("ExpectToken out of tokens\n");
        ParseError();
    }
    else if(CurrentTokenKind == TokenKind)
    {
        NextToken(Parser);
    }
    else
    {
        printf("Expected token %d but got %d %s\n", TokenKind, CurrentTokenKind, CURRENT_TOKEN(Parser).String);
        ParseError();
    }
}

static void ParseImportStar(js_parser *Parser)
{
    if(Parser->Emit) printf( "(* ");
    ExpectToken(Parser, token_kind_Star);
    ExpectToken(Parser, token_kind_As);
    ExpectToken(Parser, token_kind_Identifier);
    if(Parser->Emit) printf( "<%s> ", PREVIOUS_TOKEN(Parser).String);
    ExpectToken(Parser, token_kind_From);
    if(Parser->Emit) printf( ") ");
}

static void ParsePickName(js_parser *Parser)
{
    switch(CURRENT_TOKEN(Parser).Kind)
    {
    case token_kind_Default:
    case token_kind_String:
    case token_kind_Identifier:
        if(Parser->Emit) printf( "<%s> ", CURRENT_TOKEN(Parser).String);
        NextToken(Parser);
        break;
    default:
        if(CURRENT_TOKEN(Parser).Kind == token_kind_From)
        {
            // NOTE: there are a few import keywords that aren't actually reserved words. We
            // accept them here as a special case. There must be a better way
            NextToken(Parser);
        }
        else
        {
            printf("ParsePickName error %d %d\n", Parser->I, CURRENT_TOKEN(Parser).Kind);
            ParseError();
        }
    }
}

static void ParseAlias(js_parser *Parser)
{
    if(CURRENT_TOKEN(Parser).Kind == token_kind_As)
    {
        if(Parser->Emit) printf( "as <%s> ", CURRENT_TOKEN(Parser).String);
        NextToken(Parser);
        ExpectToken(Parser, token_kind_Identifier);
    }
}

static void ParsePickItem(js_parser *Parser)
{
    if(Parser->Emit) printf( "(");
    ParsePickName(Parser);
    ParseAlias(Parser);
    if(Parser->Emit) printf( ")");
}

static void ParsePickItems(js_parser *Parser)
{
    for(;;)
    {
        ParsePickItem(Parser);
        if(CURRENT_TOKEN(Parser).Kind == token_kind_Comma)
        {
            NextToken(Parser);
        }

        if(CURRENT_TOKEN(Parser).Kind == token_kind_CurlyClose)
        {
            NextToken(Parser);
            break;
        }
    }
}

static void ParseImportPick(js_parser *Parser)
{
    if(Parser->Emit) printf( "{");
    ExpectToken(Parser, token_kind_CurlyOpen);
    switch(CURRENT_TOKEN(Parser).Kind)
    {
    case token_kind_CurlyClose:
        NextToken(Parser);
        break;
    default:
        ParsePickItems(Parser);
    }
    if(Parser->Emit) printf( "} ");
    ExpectToken(Parser, token_kind_From);
}

static void ParseImportStarOrPick(js_parser *Parser)
{
    switch(CURRENT_TOKEN(Parser).Kind)
    {
    case token_kind_Star:
        ParseImportStar(Parser);
        break;
    case token_kind_CurlyOpen:
        ParseImportPick(Parser);
        break;
    default:
        printf("ParseImportStarOrPick\n");
        ParseError();
    }
}

static void ParseImportDefault(js_parser *Parser)
{
    if(Parser->Emit) printf( "<%s> ", CURRENT_TOKEN(Parser).String);
    ExpectToken(Parser, token_kind_Identifier);
    switch(CURRENT_TOKEN(Parser).Kind)
    {
    case token_kind_From:
        NextToken(Parser);
        break;
    case token_kind_Comma:
        NextToken(Parser);
        ParseImportStarOrPick(Parser);
        break;
    default:
        printf("ParseImportDefault default error\n");
        ParseError();
    }
}

static void ParseImportBody(js_parser *Parser)
{
    switch(CURRENT_TOKEN(Parser).Kind)
    {
    case token_kind_Identifier:
        ParseImportDefault(Parser);
        break;
    case token_kind_Star:
        ParseImportStar(Parser);
        break;
    case token_kind_CurlyOpen:
        ParseImportPick(Parser);
        break;
    default:
        printf("ParseImportBody unexpected token %d\n", CURRENT_TOKEN(Parser).Kind);
        ParseError();
    }
}

static void ParseImport(js_parser *Parser)
{
    if(Parser->Emit) printf( "(import ");
    ExpectToken(Parser, token_kind_Import);
    if(CURRENT_TOKEN(Parser).Kind != token_kind_String)
    {
        ParseImportBody(Parser);
    }
    if(Parser->Emit) printf( "<%s> ", CURRENT_TOKEN(Parser).String);
    ExpectToken(Parser, token_kind_String);
    if(Parser->Emit) printf( ") ");
    if(CURRENT_TOKEN(Parser).Kind == token_kind_SemiColon)
    {
        NextToken(Parser);
    }
}

static void ParseExprParen(js_parser *Parser)
{
    printf("ParseExprParen not implemented %d\n", Parser->I);
    ParseError();
}

static void ParseExpr(js_parser *Parser)
{
    switch(CURRENT_TOKEN(Parser).Kind)
    {
    case token_kind_ParenOpen:
        NextToken(Parser);
        ParseExprParen(Parser);
        break;
    default:
        printf("ParseExpr unexpected token %d\n", CURRENT_TOKEN(Parser).Kind);
        ParseError();
    }
    // TODO: parse expr
    // Expr = ExprParen | ExprValue
    // ExprParen = /(/ Expr /)/
    // ExprValue = ArithmeticExpr | Function | FunctionCall
}

static void ParseJs(js_parser *Parser)
{
    b32 Running = 1;
    while(Running)
    {
        switch(CURRENT_TOKEN(Parser).Kind)
        {
        case token_kind_Import:
            ParseImport(Parser);
            /* vec_push(Result, Range); */
            if(Parser->Emit) printf( "\n");
            break;
        case token_kind_None:
            Running = 0;
            break;
        default:
            ParseExpr(Parser);
        }
    }
}

static b32 IsJsFile(char *FilePath)
{
    s32 Length = -1;
    while(FilePath[++Length]);
    b32 Result = (FilePath[Length-3] && FilePath[Length-3] == '.' &&
                  FilePath[Length-2] && (FilePath[Length-2] == 'j' || FilePath[Length-2] == 't') &&
                  FilePath[Length-1] && FilePath[Length-1] == 's');
    return Result;
}

void TestParseJs()
{
    file_info *FileInfo = FileTreeWalk("../test");
    s32 I;
    printf("digraph DepGraph {\n");
    printf("    rankdir=\"BT\";\n");
    for(I = 0; FileInfo[I].fpath != 0; ++I)
    {
        if(IsJsFile(FileInfo[I].fpath))
        {
            buffer *Source = ReadFileIntoBuffer(FileInfo[I].fpath);
            if(!Source)
            {
                continue;
            }
            lexer Lexer = {*Source,0};
            token *Tokens = LexJs(&Lexer);
            js_parser Parser = {vec_len(Tokens),0,Tokens,0};
            ParseJs(&Parser);
#if 0
            // TODO: use this loop again once ParseJs returns something iterable
            u32 K;
            for(K = 0; K < vec_len(Ranges); ++K)
            {
                char * Range = Ranges[K];
                char Path[1 + Range.End - Range.Start];
                Path[Range.End - Range.Start] = 0;
                memcpy(Path, Source->Data + Range.Start, Range.End - Range.Start);
                char *ResolvedPath = ResolvePath(FileInfo[I].fpath, Path);
                printf("    \"%s\"->\"%s\";\n", FileInfo[I].fpath, ResolvedPath);
            }
#endif
        }
    }
    printf("}\n");
}
