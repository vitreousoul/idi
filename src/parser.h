typedef enum ascii_code
{
    AsciiCodeNull = 0,
    AsciiCodeStartOfHeading = 1,
    AsciiCodeStartOfText = 2,
    AsciiCodeEndOfText = 3,
    AsciiCodeEndOfTransmission = 4,
    AsciiCodeEnquery = 5,
    AsciiCodeAcknowledge = 6,
    AsciiCodeBell = 7,
    AsciiCodeBackSpace = 8,
    AsciiCodeHorizontalTab = 9,
    AsciiCodeLineFeed = 10,
    AsciiCodeVerticalTab = 11,
    AsciiCodeFormFeed = 12,
    AsciiCodeCarriageReturn = 13,
    AsciiCodeShiftOut = 14,
    AsciiCodeShiftIn = 15,
    AsciiCodeDataLinkEscape = 16,
    AsciiCodeDeviceControl1 = 17,
    AsciiCodeDeviceControl2 = 18,
    AsciiCodeDeviceControl3 = 19,
    AsciiCodeDeviceControl4 = 20,
    AsciiCodeNegativeAcknowledge = 21,
    AsciiCodeSynchronousIdle = 22,
    AsciiCodeEndOfTransmissionBlock = 23,
    AsciiCodeCancel = 24,
    AsciiCodeEndOfMedium = 25,
    AsciiCodeSubstitute = 26,
    AsciiCodeEscape = 27,
    AsciiCodeFileSeparator = 28,
    AsciiCodeGroupSeparator = 29,
    AsciiCodeRecordSeparator = 30,
    AsciiCodeUnitSeparator = 31,
    AsciiCodeSpace = 32,
    AsciiCodeBang = 33,
    AsciiCodeDoubleQuote = 34,
    AsciiCodeHash = 35,
    AsciiCodeDollar = 36,
    AsciiCodePercent = 37,
    AsciiCodeAmpersand = 38,
    AsciiCodeSingleQuote = 39,
    AsciiCodeOpenParenthesis = 40,
    AsciiCodeCloseParenthesis = 41,
    AsciiCodeStar = 42,
    AsciiCodePlus = 43,
    AsciiCodeComma = 44,
    AsciiCodeDash = 45,
    AsciiCodePeriod = 46,
    AsciiCodeForwardSlash = 47,
    AsciiCode0 = 48,
    AsciiCode1 = 49,
    AsciiCode2 = 50,
    AsciiCode3 = 51,
    AsciiCode4 = 52,
    AsciiCode5 = 53,
    AsciiCode6 = 54,
    AsciiCode7 = 55,
    AsciiCode8 = 56,
    AsciiCode9 = 57,
    AsciiCodeColon = 58,
    AsciiCodeSemiColon = 59,
    AsciiCodeLessThan = 60,
    AsciiCodeEquals = 61,
    AsciiCodeGreaterThan = 62,
    AsciiCodeQuestionMark = 63,
    AsciiCodeAt = 64,
    AsciiCodeA = 65,
    AsciiCodeB = 66,
    AsciiCodeC = 67,
    AsciiCodeD = 68,
    AsciiCodeE = 69,
    AsciiCodeF = 70,
    AsciiCodeG = 71,
    AsciiCodeH = 72,
    AsciiCodeI = 73,
    AsciiCodeJ = 74,
    AsciiCodeK = 75,
    AsciiCodeL = 76,
    AsciiCodeM = 77,
    AsciiCodeN = 78,
    AsciiCodeO = 79,
    AsciiCodeP = 80,
    AsciiCodeQ = 81,
    AsciiCodeR = 82,
    AsciiCodeS = 83,
    AsciiCodeT = 84,
    AsciiCodeU = 85,
    AsciiCodeV = 86,
    AsciiCodeW = 87,
    AsciiCodeX = 88,
    AsciiCodeY = 89,
    AsciiCodeZ = 90,
    AsciiCodeOpenBracket = 91,
    AsciiCodeBackSlash = 92,
    AsciiCodeCloseBracket = 93,
    AsciiCodeCarrot = 94,
    AsciiCodeUnderScore = 95,
    AsciiCodeBackTick = 96,
    AsciiCodea = 97,
    AsciiCodeb = 98,
    AsciiCodec = 99,
    AsciiCoded = 100,
    AsciiCodee = 101,
    AsciiCodef = 102,
    AsciiCodeg = 103,
    AsciiCodeh = 104,
    AsciiCodei = 105,
    AsciiCodej = 106,
    AsciiCodek = 107,
    AsciiCodel = 108,
    AsciiCodem = 109,
    AsciiCoden = 110,
    AsciiCodeo = 111,
    AsciiCodep = 112,
    AsciiCodeq = 113,
    AsciiCoder = 114,
    AsciiCodes = 115,
    AsciiCodet = 116,
    AsciiCodeu = 117,
    AsciiCodev = 118,
    AsciiCodew = 119,
    AsciiCodex = 120,
    AsciiCodey = 121,
    AsciiCodez = 122,
    AsciiCodeOpenBrace = 123,
    AsciiCodePipe = 124,
    AsciiCodeCloseBrace = 125,
    AsciiCodeTolde = 126,
    AsciiCodeDelete = 127,
} ascii_code;

typedef enum parse_tree_state
{
    ParseTreeStateRunning,
    ParseTreeStateSuccess,
    ParseTreeStateError,
} parse_tree_state;

typedef enum parse_tree_type
{
    ParseTreeTypeTextMatch,
    ParseTreeTypeCharSet,
    ParseTreeTypeCharRange,
    ParseTreeTypeAnd,
    ParseTreeTypeOr,
} parse_tree_type;

typedef struct text_match_node
{
    buffer *Text;
    size Index;
} text_match_node;

typedef struct char_set_node
{
    b32 Exclusive;
    buffer *Text;
} char_set_node;

typedef struct char_range
{
    char Begin;
    char End;
} char_range;

typedef struct string_literal_node
{
    b32 InsideString;
} string_literal_node;

typedef struct parse_tree
{
    parse_tree_type Type;
    parse_tree_state State;
    u32 NodeCount;

    u32 RepeatMin;
    u32 RepeatMax;
    u32 RepeatCount;

    b32 HasEntryIndex;
    size EntryIndex;

    b32 ConsumeWhitespace;

    u32 DebugId;

    union
    {
        text_match_node *TextMatch;
        char_set_node *CharSet;
        string_literal_node *StringLiteral;
        char_range *CharRange;
        struct parse_tree *Nodes;
    } Value;
} parse_tree;

typedef struct parser
{
    size Index;
} parser;

const char *DisplayParseTreeState(parse_tree *ParseTree);
parse_tree ParseBuffer(buffer *Buffer);
