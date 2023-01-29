#include "idi.h"

#include "buffer.c"
#include "platform.c"
#include "json_parser.c"
#include "js_lex.c"

#if BUILD_GUI
#include "gui.c"
#endif

int main()
{
    int Result = 0;

    if(vec_test() != 0)
    {
        printf("[ error ] vec_test\n");
    }

    if(TestJsLex() != 0)
    {
        printf("[ error ] TestJsLex\n");
    }

    buffer *Source = ReadFileIntoBuffer("./idi.idi");
    printf("\nParseJson:\n");
    ParseJson(Source);
#if BUILD_GUI
    /* DisplayWindow(); */
    DrawTree();
#endif
    FreeBuffer(Source);

    return Result;
}
