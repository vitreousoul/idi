#include "idi.h"

#include "buffer.c"
#include "platform.c"
#include "json_parser.c"

#if BUILD_GUI
#include "gui.c"
#endif

int main()
{
    int Result = 0;

    if(buf_test() != 0)
    {
        printf("[ error ] buf_test");
    }

    buffer *Source = ReadFileIntoBuffer("./idi.idi");
    ParseJson(Source);

#if BUILD_GUI
    /* DisplayWindow(); */
    DrawTree();
#endif
    FreeBuffer(Source);

    return Result;
}
