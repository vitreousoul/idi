// ==========
//   Grammar
// ==========
//
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
//

#define CURRENT_TOKEN(Parser) (HAS_TOKENS(Parser) ? (Parser)->Tokens[(Parser)->I] : EmptyToken())
#define HAS_TOKENS(Parser) ((Parser)->I < (Parser)->TokenCount)

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
        printf("Out of tokens");
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
        printf("Expected token %d but got %d %lu %lu\n", TokenKind, CurrentTokenKind, CURRENT_TOKEN(Parser).String.Start, CURRENT_TOKEN(Parser).String.End);
        ParseError();
    }
}

static void ParseImportStar(js_parser *Parser)
{
    ExpectToken(Parser, token_kind_Star);
    ExpectToken(Parser, token_kind_As);
    ExpectToken(Parser, token_kind_Identifier);
    ExpectToken(Parser, token_kind_From);
}

static void ParsePickName(js_parser *Parser)
{
    switch(CURRENT_TOKEN(Parser).Kind)
    {
    case token_kind_Default:
    case token_kind_String:
    case token_kind_Identifier:
        NextToken(Parser);
        break;
    default:
        printf("ParsePickName\n");
        ParseError();
    }
}

static void ParseAlias(js_parser *Parser)
{
    if(CURRENT_TOKEN(Parser).Kind == token_kind_As)
    {
        NextToken(Parser);
        ExpectToken(Parser, token_kind_Identifier);
    }
}

static void ParsePickItem(js_parser *Parser)
{
    ParsePickName(Parser);
    ParseAlias(Parser);
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
    ExpectToken(Parser, token_kind_CurlyOpen);
    switch(CURRENT_TOKEN(Parser).Kind)
    {
    case token_kind_CurlyClose:
        NextToken(Parser);
        break;
    default:
        ParsePickItems(Parser);
    }
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
    ExpectToken(Parser, token_kind_Import);
    if(CURRENT_TOKEN(Parser).Kind != token_kind_String)
    {
        ParseImportBody(Parser);
    }
    ExpectToken(Parser, token_kind_String);
    ExpectToken(Parser, token_kind_SemiColon);
}

static void ParseJs(js_parser *Parser)
{
    for(;;)
    {
        if(CURRENT_TOKEN(Parser).Kind == token_kind_Import)
        {
            ParseImport(Parser);
        }
        else
        {
            printf("End of top imports\n");
            break;
        }
    }
}

void TestParseJs()
{
    buffer *Source = ReadFileIntoBuffer("../test/test.js");
    lexer Lexer = {*Source,0};
    token *Tokens = LexJs(&Lexer);
    js_parser Parser = {vec_len(Tokens),0,Tokens};
    file_info *FileInfo = FileTreeWalk("../src");
    ParseJs(&Parser);
    printf("Parser[%d/%d]\n", Parser.I, Parser.TokenCount);
    assert(Parser.I == Parser.TokenCount);
}
