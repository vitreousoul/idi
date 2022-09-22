#define ArrayItemSize(Array) (sizeof(Array[0]))
#define ArrayCount(Array) (sizeof(Array) / ArrayItemSize(Array))

#define CharIsUpperCase(Char) (Char >= 'A' && Char <= 'Z')
#define CharIsLowerCase(Char) (Char >= 'a' && Char <= 'z')
#define CharIsDigit(Char) (Char >= '0' && Char <= '9')
#define CharIsAlphaNum(Char) (CharIsUpperCase(Char) || CharIsLowerCase(Char) || CharIsDigit(Char))
#define CharIsSpace(Char) ((Char == ' ') || (Char == '\n') || (Char == '\r') || (Char == '\t'))

u32 IndentationCount = 0;
u32 IndentationSize = 4;

static void
Log(const char *Message)
{
    printf("%s\n", Message);
}

static text_match_node *
CreateTextMatchNode(const char *Text)
{
    buffer *BufferText = BufferFromNullTerminatedString(Text);
    text_match_node *Result = malloc(sizeof(text_match_node));
    Result->Text = BufferText;
    Result->Index = 0;

    return Result;
}

static char_set_node *
CreateCharSetNode(const char *CharSet, b32 Exclusive)
{
    buffer *BufferText = BufferFromNullTerminatedString(CharSet);
    char_set_node *Result = malloc(sizeof(text_match_node));
    Result->Exclusive = Exclusive;
    Result->Text = BufferText;

    return Result;
}

static parse_tree
CreateTextMatchParseTree(const char *Text)
{
    parse_tree Result;
    Result.Type = ParseTreeTypeTextMatch;
    Result.State = ParseTreeStateRunning;
    Result.NodeCount = 0;

    Result.RepeatMin = 1;
    Result.RepeatMax = 1;
    Result.RepeatCount = 0;

    Result.HasEntryIndex = 0;
    Result.EntryIndex = 0;

    Result.ConsumeWhitespace = 0;

    Result.Value.TextMatch = CreateTextMatchNode(Text);

    return Result;
}

static parse_tree
CreateCharSetParseTree(const char *CharSet, b32 Exclusive)
{
    parse_tree Result;
    Result.Type = ParseTreeTypeCharSet;
    Result.State = ParseTreeStateRunning;
    Result.NodeCount = 0;

    Result.RepeatMin = 1;
    Result.RepeatMax = 1;
    Result.RepeatCount = 0;

    Result.HasEntryIndex = 0;
    Result.EntryIndex = 0;

    Result.ConsumeWhitespace = 0;

    Result.Value.CharSet = CreateCharSetNode(CharSet, Exclusive);

    return Result;
}

static parse_tree
CreateCharRangeParseTree(char Begin, char End)
{
    parse_tree Result;
    Result.Type = ParseTreeTypeCharRange;
    Result.State = ParseTreeStateRunning;
    Result.NodeCount = 0;

    Result.RepeatMin = 1;
    Result.RepeatMax = 1;
    Result.RepeatCount = 0;

    Result.HasEntryIndex = 0;
    Result.EntryIndex = 0;

    Result.ConsumeWhitespace = 0;

    char_range *CharRange = malloc(sizeof(char_range));
    CharRange->Begin = Begin;
    CharRange->End = End;
    Result.Value.CharRange = CharRange;

    return Result;
}

static parse_tree
CreateAndParseTree(u32 NodeCount, parse_tree *Nodes)
{
    parse_tree Result;
    Result.Type = ParseTreeTypeAnd;
    Result.State = ParseTreeStateRunning;
    Result.NodeCount = NodeCount;

    Result.RepeatMin = 1;
    Result.RepeatMax = 1;
    Result.RepeatCount = 0;

    Result.HasEntryIndex = 0;
    Result.EntryIndex = 0;

    Result.ConsumeWhitespace = 0;

    Result.Value.Nodes = Nodes;

    return Result;
}

static parse_tree
CreateOrParseTree(u32 NodeCount, parse_tree *Nodes)
{
    parse_tree Result;
    Result.Type = ParseTreeTypeOr;
    Result.State = ParseTreeStateRunning;
    Result.NodeCount = NodeCount;

    Result.RepeatMin = 1;
    Result.RepeatMax = 1;
    Result.RepeatCount = 0;

    Result.HasEntryIndex = 0;
    Result.EntryIndex = 0;

    Result.ConsumeWhitespace = 0;

    Result.Value.Nodes = Nodes;

    return Result;
}

static parse_tree *
GetParseTreeNode(parse_tree *ParseTree, size Index)
{
    return &ParseTree->Value.Nodes[Index];
}

static size
GetCharSetTextSize(parse_tree *ParseTree)
{
    size Result = ParseTree->Value.CharSet->Text->Size;
    return Result;
}

static char
GetCharSetChar(parse_tree *ParseTree, size Index)
{
    char Result = ParseTree->Value.CharSet->Text->Data[Index];

    return Result;
}

static u8
GetTextMatchCharacter(text_match_node *TextMatchNode)
{
    u8 Result = '\0';

    if (TextMatchNode->Index < TextMatchNode->Text->Size)
    {
        Result = TextMatchNode->Text->Data[TextMatchNode->Index];
    }

    return Result;
}

const char *
DisplayParseTreeState(parse_tree *ParseTree)
{
    switch(ParseTree->State)
    {
    case ParseTreeStateRunning: return "ParseTreeStateRunning";
    case ParseTreeStateSuccess: return "ParseTreeStateSuccess";
    case ParseTreeStateError: return "ParseTreeStateError";
    }
}

static const char *
DisplayParseTreeType(parse_tree *ParseTree)
{
    switch(ParseTree->Type)
    {
    case ParseTreeTypeTextMatch: return "ParseTreeTypeTextMatch";
    case ParseTreeTypeCharSet: return "ParseTreeTypeCharSet";
    case ParseTreeTypeCharRange: return "ParseTreeTypeCharRange";
    case ParseTreeTypeAnd: return "ParseTreeTypeAnd";
    case ParseTreeTypeOr: return "ParseTreeTypeOr";
    }
}

static void
ConsumeWhitespace(parser *Parser, buffer *Buffer)
{
    u32 SpaceCount = 0;
    while(CharIsSpace(Buffer->Data[Parser->Index]))
    {
        ++SpaceCount;
        ++Parser->Index;
    }
    // TODO: fix this decrement hack. it's only here because we always increment the parser
    // index after calling StepParser in the ParseBuffer loop
    --Parser->Index;
}

static void
ResetParseTree(parse_tree *ParseTree)
{
    ParseTree->State = ParseTreeStateRunning;

    switch(ParseTree->Type)
    {
    case ParseTreeTypeTextMatch:
    {
        DebugPrintBuffer(ParseTree->Value.TextMatch->Text);
        ParseTree->Value.TextMatch->Index = 0;
    } break;
    case ParseTreeTypeCharSet:
    {
    } break;
    case ParseTreeTypeCharRange:
    {
    } break;
    case ParseTreeTypeAnd:
    case ParseTreeTypeOr:
    {
        for(size Index = 0; Index < ParseTree->NodeCount; Index++)
        {
            ResetParseTree(&ParseTree->Value.Nodes[Index]);
        }
    } break;
    }
}

static void
StepParseTree(parser *Parser, parse_tree *ParseTree, buffer *Buffer)
{
    IndentationCount += IndentationSize;
    u8 Character = Buffer->Data[Parser->Index];

    switch(ParseTree->Type)
    {
    case ParseTreeTypeTextMatch:
    {
        u8 TextMatchChar = GetTextMatchCharacter(ParseTree->Value.TextMatch);

        if(Character == TextMatchChar)
        {
            ++ParseTree->Value.TextMatch->Index;

            if(GetTextMatchCharacter(ParseTree->Value.TextMatch) == '\0')
            {
                ParseTree->State =  ParseTreeStateSuccess;
            }
        }
        else
        {
            ParseTree->State =  ParseTreeStateError;
        }
    } break;

    case ParseTreeTypeCharSet:
    {
        b32 Match = 0;

        for(size Index = 0; Index < GetCharSetTextSize(ParseTree); Index++)
        {
            if (GetCharSetChar(ParseTree, Index) == Character)
            {
                Match = 1;
                break;
            }
        }

        if(ParseTree->Value.CharSet->Exclusive)
        {
            Match = !Match;
        }

        if (Match)
        {
            ParseTree->State =  ParseTreeStateSuccess;
        }
        else
        {
            ParseTree->State =  ParseTreeStateError;
        }
    } break;

    case ParseTreeTypeCharRange:
    {
        if((Character >= ParseTree->Value.CharRange->Begin &&
            Character <= ParseTree->Value.CharRange->End))
        {
            ParseTree->State =  ParseTreeStateSuccess;
        }
        else
        {
            ParseTree->State =  ParseTreeStateError;
        }
    } break;

    case ParseTreeTypeAnd:
    {
        b32 AllSuccess = True;

        for(size Index = 0; Index < ParseTree->NodeCount; Index++)
        {
            parse_tree *Node = GetParseTreeNode(ParseTree, Index);

            if(Node->State == ParseTreeStateError)
            {
                ParseTree->State =  ParseTreeStateError;
                AllSuccess = False;
                break;
            }
            else if(Node->State == ParseTreeStateRunning)
            {
                StepParseTree(Parser, Node, Buffer);

                if(Index != (ParseTree->NodeCount - 1) || Node->State != ParseTreeStateSuccess)
                {
                    AllSuccess = False;
                    break;
                }
            }
        }

        if(AllSuccess)
        {
            ParseTree->State = ParseTreeStateSuccess;
        }
    } break;

    case ParseTreeTypeOr:
    {
        b32 AllError = True;

        for(size Index = 0; Index < ParseTree->NodeCount; Index++)
        {
            parse_tree *Node = GetParseTreeNode(ParseTree, Index);

            if(Node->State == ParseTreeStateRunning)
            {
                StepParseTree(Parser, Node, Buffer);
            }

            if(Node->State == ParseTreeStateSuccess)
            {
                ParseTree->State = ParseTreeStateSuccess;
                AllError = False;
                break;
            }
            else if(Node->State != ParseTreeStateError)
            {
                AllError = False;
            }
        }

        if(AllError)
        {
            ParseTree->State =  ParseTreeStateError;
        }
    } break;

    default:
    {
        Log("Error: ParseTree state default case error\n");
        ParseTree->State = ParseTreeStateError;
    } break;
    }

    if(ParseTree->State == ParseTreeStateSuccess)
    {
        ParseTree->RepeatCount = ParseTree->RepeatCount + 1;

        if(ParseTree->RepeatCount < ParseTree->RepeatMax)
        {
            ParseTree->State = ParseTreeStateRunning;
            ResetParseTree(ParseTree);
        }

        if(ParseTree->RepeatMin == 0)
        {
            ParseTree->EntryIndex = Parser->Index;
        }
    }
    else if(ParseTree->State == ParseTreeStateError)
    {
        if (ParseTree->RepeatMax > 1)
        {
            ParseTree->State = ParseTreeStateSuccess;
            Parser->Index -= Parser->Index - ParseTree->EntryIndex;
        }
    }

    if(ParseTree->State == ParseTreeStateSuccess && ParseTree->ConsumeWhitespace)
    {
        Parser->Index++;
        ConsumeWhitespace(Parser, Buffer);
    }

    IndentationCount -= IndentationSize;
}

static parse_tree
CreateTitleStringParseTree()
{
    parse_tree *AlphaNodes = malloc(sizeof(parse_tree) * 2);
    AlphaNodes[0] = CreateCharRangeParseTree('A', 'Z');
    AlphaNodes[1] = CreateCharRangeParseTree('a', 'z');
    parse_tree AlphaParseTree = CreateOrParseTree(2, AlphaNodes);
    AlphaParseTree.RepeatMin = 0;
    AlphaParseTree.RepeatMax = ~0;
    parse_tree *ResultNodes = malloc(sizeof(parse_tree) * 2);
    ResultNodes[0] = CreateCharRangeParseTree('A', 'Z');
    ResultNodes[1] = AlphaParseTree;
    parse_tree Result = CreateAndParseTree(2, ResultNodes);

    return Result;
}

static parse_tree
CreateStringLiteralParseTree()
{
    parse_tree *AndNodes = malloc(sizeof(parse_tree) * 3);
    AndNodes[0] = CreateCharSetParseTree("\"", 0);
    AndNodes[1] = CreateCharSetParseTree("\"", 1);
    AndNodes[1].RepeatMin = 0;
    AndNodes[1].RepeatMax = ~0;
    AndNodes[2] = CreateCharSetParseTree("\"", 0);
    parse_tree Result = CreateAndParseTree(3, AndNodes);

    return Result;
}

static parse_tree
CreateIdiParseTree()
{
    parse_tree *BindSetNodes = malloc(sizeof(parse_tree) * 3);
    BindSetNodes[0] = CreateTextMatchParseTree("set");
    BindSetNodes[0].ConsumeWhitespace = 1;
    BindSetNodes[1] = CreateTitleStringParseTree();
    BindSetNodes[1].ConsumeWhitespace = 1;
    BindSetNodes[2] = CreateStringLiteralParseTree();
    parse_tree Result = CreateAndParseTree(3, BindSetNodes);

    return Result;
}

parse_tree
ParseBuffer(buffer *Buffer)
{
    parse_tree ParseTree = CreateIdiParseTree();

    parser Parser = {0};

    const u32 MaxIterCount = 1000;
    u32 Iter = 0;

    Log("begin parse loop\n");
    while(ParseTree.State == ParseTreeStateRunning && (Iter++ < MaxIterCount))
    {
        if(Parser.Index >= Buffer->Size)
        {
            if(ParseTree.State != ParseTreeStateSuccess)
            {
                ParseTree.State =  ParseTreeStateError;
            }
        }
        else if(Iter == MaxIterCount)
        {
            Log("\nMax iterations\n");
            ParseTree.State =  ParseTreeStateError;
        }
        else
        {
            StepParseTree(&Parser, &ParseTree, Buffer);
            ++Parser.Index;
        }
    }

    for(size I = 0; I < Parser.Index; I++) { printf("%c", Buffer->Data[I]); }
    printf("\n");

    return ParseTree;
}
