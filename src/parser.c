#include <stdio.h>

#include "parser.h"

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
CreateCharSetNode(const char *CharSet)
{
    buffer *BufferText = BufferFromNullTerminatedString(CharSet);
    char_set_node *Result = malloc(sizeof(text_match_node));
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
    Result.Value.TextMatch = CreateTextMatchNode(Text);

    return Result;
}

static parse_tree
CreateCharSetParseTree(const char *CharSet)
{
    parse_tree Result;
    Result.Type = ParseTreeTypeCharSet;
    Result.State = ParseTreeStateRunning;
    Result.NodeCount = 0;
    Result.Value.CharSet = CreateCharSetNode(CharSet);

    return Result;
}

static parse_tree
CreateCharRangeParseTree(char Begin, char End)
{
    parse_tree Result;
    Result.Type = ParseTreeTypeCharRange;
    Result.State = ParseTreeStateRunning;
    Result.NodeCount = 0;
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
    Result.Value.Nodes = Nodes;

    return Result;
}

parse_tree_state
GetParseTreeState(parse_tree *ParseTree)
{
    parse_tree_state Result = ParseTree->State;

    return Result;
}

parse_tree_type
GetParseTreeType(parse_tree *ParseTree)
{
    parse_tree_type Result = ParseTree->Type;

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

static void
SetParseTreeState(parse_tree *ParseTree, parse_tree_state State)
{
    ParseTree->State = State;
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
    switch(GetParseTreeState(ParseTree))
    {
        case ParseTreeStateRunning: return "ParseTreeStateRunning";
        case ParseTreeStateSuccess: return "ParseTreeStateSuccess";
        case ParseTreeStateError: return "ParseTreeStateError";
    }
}

const char *
DisplayParseTreeType(parse_tree *ParseTree)
{
    switch(GetParseTreeType(ParseTree))
    {
        case ParseTreeTypeTextMatch: return "ParseTreeTypeTextMatch";
        case ParseTreeTypeCharSet: return "ParseTreeTypeCharSet";
        case ParseTreeTypeCharRange: return "ParseTreeTypeCharRange";
        case ParseTreeTypeAnd: return "ParseTreeTypeAnd";
        case ParseTreeTypeOr: return "ParseTreeTypeOr";
    }
}

static void
StepParseTree(parse_tree *ParseTree, u8 Character)
{
    switch(GetParseTreeType(ParseTree))
    {
        case ParseTreeTypeTextMatch:
        {
            u8 TextMatchChar = GetTextMatchCharacter(ParseTree->Value.TextMatch);

            if(Character == TextMatchChar)
            {
                ++ParseTree->Value.TextMatch->Index;

                if(GetTextMatchCharacter(ParseTree->Value.TextMatch) == '\0')
                {
                    SetParseTreeState(ParseTree, ParseTreeStateSuccess);
                }
            }
            else
            {
                SetParseTreeState(ParseTree, ParseTreeStateError);
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

            if (Match)
            {
                SetParseTreeState(ParseTree, ParseTreeStateSuccess);
            }
            else
            {
                SetParseTreeState(ParseTree, ParseTreeStateError);
            }
        } break;

        case ParseTreeTypeCharRange:
        {
            if((Character >= ParseTree->Value.CharRange->Begin &&
                Character <= ParseTree->Value.CharRange->End))
            {
                SetParseTreeState(ParseTree, ParseTreeStateSuccess);
            }
            else
            {
                SetParseTreeState(ParseTree, ParseTreeStateError);
            }
        } break;

        case ParseTreeTypeAnd:
        {
            b32 AllSuccess = True;

            for(size Index = 0; Index < ParseTree->NodeCount; Index++)
            {
                parse_tree *Node = GetParseTreeNode(ParseTree, Index);

                if(GetParseTreeState(Node) == ParseTreeStateError)
                {
                    SetParseTreeState(ParseTree, ParseTreeStateError);
                    AllSuccess = False;
                    break;
                }
                else if(GetParseTreeState(Node) == ParseTreeStateRunning)
                {
                    StepParseTree(Node, Character);
                    AllSuccess = False;
                    break;
                }
            }

            if(AllSuccess)
            {
                SetParseTreeState(ParseTree, ParseTreeStateSuccess);
            }
        } break;

        case ParseTreeTypeOr:
        {
            b32 AllError = True;

            for(size Index = 0; Index < ParseTree->NodeCount; Index++)
            {
                parse_tree *Node = GetParseTreeNode(ParseTree, Index);

                if(GetParseTreeState(Node) == ParseTreeStateRunning)
                {
                    StepParseTree(Node, Character);
                }

                if(GetParseTreeState(Node) == ParseTreeStateSuccess)
                {
                    SetParseTreeState(ParseTree, ParseTreeStateSuccess);
                    AllError = False;
                    break;
                }
                else if(GetParseTreeState(Node) != ParseTreeStateError)
                {
                    AllError = False;
                }
            }

            if(AllError)
            {
                SetParseTreeState(ParseTree, ParseTreeStateError);
            }
        } break;

        default:
        {
            printf("Error: ParseTree state default case error\n");
            SetParseTreeState(ParseTree, ParseTreeStateError);
        } break;
    }
}

parse_tree
CreateTitleStringParseTree()
{
    parse_tree *AlphaNodes = malloc(sizeof(parse_tree) * 2);
    AlphaNodes[0] = CreateCharRangeParseTree('A', 'Z');
    AlphaNodes[1] = CreateCharRangeParseTree('a', 'z');
    parse_tree AlphaParseTree = CreateOrParseTree(2, AlphaNodes);
    parse_tree *ResultNodes = malloc(sizeof(parse_tree) * 2);
    ResultNodes[0] = CreateCharRangeParseTree('A', 'Z');
    ResultNodes[1] = AlphaParseTree;
    parse_tree Result = CreateAndParseTree(2, ResultNodes);

    return Result;
}

parse_tree
CreateDebugParseTree()
{
    parse_tree *FirstNodes = malloc(sizeof(parse_tree) * 2);
    FirstNodes[0] = CreateTextMatchParseTree("foo");
    FirstNodes[1] = CreateTextMatchParseTree("bar");
    parse_tree FirstPartOfOr = CreateAndParseTree(2, FirstNodes);
    parse_tree SecondPartOfOr = CreateTextMatchParseTree("baz");
    parse_tree *SecondNodes = malloc(sizeof(parse_tree) * 2);
    SecondNodes[0] = FirstPartOfOr;
    SecondNodes[1] = SecondPartOfOr;
    parse_tree Result = CreateOrParseTree(2, SecondNodes);

    return Result;
}

parse_tree
ParseBuffer(buffer *Buffer)
{
    // TODO: free stuff we malloced in here >:(  !!!!!
    /* parse_tree ParseTree = CreateDebugParseTree(); */
    /* parse_tree ParseTree = CreateCharSetParseTree("abcd"); */
    parse_tree ParseTree = CreateTitleStringParseTree();

    parser Parser = {0};

    const u32 MaxIterCount = 9;
    u32 Iter = 0;

    printf("begin parse loop\n");
    while(GetParseTreeState(&ParseTree) == ParseTreeStateRunning && (Iter++ < MaxIterCount))
    {
        if(Parser.Index >= Buffer->Size)
        {
            if(GetParseTreeState(&ParseTree) != ParseTreeStateSuccess)
            {
                SetParseTreeState(&ParseTree, ParseTreeStateError);
            }
        }
        else if(Iter == MaxIterCount)
        {
            printf("\nMax iterations\n");
            SetParseTreeState(&ParseTree, ParseTreeStateError);
        }
        else
        {
            StepParseTree(&ParseTree, Buffer->Data[Parser.Index]);
            ++Parser.Index;
        }
    }

    return ParseTree;
}
