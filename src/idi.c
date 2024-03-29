#include "idi.h"

#include "ds.c"
#include "buffer.c"
#include "platform.c"
#include "json_parser.c"
#include "js_lex.c"
#include "js_parse.c"

#ifdef BUILD_GUI
#include "gui.c"
#endif

#define DEBUG_VEC_TEST 0
#define DEBUG_JS_LEX_TEST 0
#define DEBUG_JS_PARSE_TEST 1
#define DEBUG_PARSE_JSON_TEST 0

int main()
{
    int Result = 0;

#if DEBUG_VEC_TEST
    if(vec_test() != 0)
    {
        printf("[ error ] vec_test\n");
    }
#endif

#if DEBUG_JS_LEX_TEST
    if(TestJsLex() != 0)
    {
        printf("[ error ] TestJsLex\n");
    }
#endif

#if DEBUG_VEC_TEST
    {
        buffer *Source = ReadFileIntoBuffer("../idi.idi");
        printf("\nParseJson:\n");
        ParseJson(Source);
        FreeBuffer(Source);
    }
#endif


    TestHashTable();

#if DEBUG_JS_PARSE_TEST
    TestParseJs();
#endif

#ifdef BUILD_GUI
    printf("displaying window");
    DisplayWindow();
#endif

    return Result;
}
