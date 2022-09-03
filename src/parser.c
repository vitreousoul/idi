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
CreateAndParseTree(u32 NodeCount, parse_tree *Nodes)
{
    parse_tree Result;
    Result.Type = ParseTreeTypeAnd;
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
        case ParseTreeTypeAnd: return "ParseTreeTypeAnd";
    }
}

static void
StepParseTree(parse_tree *ParseTree, parser *Parser, u8 Character)
{
    printf("StepParseTree\n");
    switch(GetParseTreeType(ParseTree))
    {
        case ParseTreeTypeTextMatch:
        {
            u8 TextMatchChar = GetTextMatchCharacter(ParseTree->Value.TextMatch);

            if(TextMatchChar == '\0')
            {
                SetParseTreeState(ParseTree, ParseTreeStateSuccess);
            }
            else if(Character == TextMatchChar)
            {
                ++ParseTree->Value.TextMatch->Index;
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
                parse_tree_state State = GetParseTreeState(Node);

                if(State == ParseTreeStateError)
                {
                    SetParseTreeState(ParseTree, ParseTreeStateError);
                    AllSuccess = False;
                    break;
                }
                else if(State == ParseTreeStateRunning)
                {
                    StepParseTree(Node, Parser, Character);

                    if(GetParseTreeState(Node) != ParseTreeStateSuccess)
                    {
                        AllSuccess = False;
                        break;
                    }
                }
                else if (State == ParseTreeStateSuccess)
                {
                    continue;
                }
            }

            if(AllSuccess)
            {
                SetParseTreeState(ParseTree, ParseTreeStateSuccess);
            }
            else
            {
                ++Parser->Index;
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
CreateDebugParseTree()
{
    parse_tree *Nodes = malloc(sizeof(parse_tree) * 2);
    Nodes[0] = CreateTextMatchParseTree("foo");
    Nodes[1] = CreateTextMatchParseTree("bar");
    parse_tree Result = CreateAndParseTree(2, Nodes);

    return Result;
}

parse_tree
ParseBuffer(buffer *Buffer)
{
    // TODO: free stuff we malloced in here >:(  !!!!!
    parse_tree ParseTree = CreateDebugParseTree();
    parser Parser = {0};

    const u32 MaxIterCount = 9;
    u32 Iter = 0;

    printf("begin parse loop\n");
    while(GetParseTreeState(&ParseTree) == ParseTreeStateRunning && (Iter++ < MaxIterCount))
    {
        if(Parser.Index >= Buffer->Size)
        {
            printf("End of buffer\n");

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
            printf("\nNext char = \"%c\"\n", Buffer->Data[Parser.Index]);
            StepParseTree(&ParseTree, &Parser, Buffer->Data[Parser.Index]);
        }
    }

    return ParseTree;
}
