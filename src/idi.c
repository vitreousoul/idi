#include "idi.h"

#include "ds.c"
#include "buffer.c"
#include "platform.c"
#include "json_parser.c"
#include "js_lex.c"

#if BUILD_GUI
#include "gui.c"
#endif

#define DEBUG_VEC_TEST 0
#define DEBUG_JS_LEX_TEST 1
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


#if BUILD_GUI
    DisplayWindow();
#endif

    TestHashTable();

    return Result;
}
