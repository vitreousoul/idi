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

    buffer *Source = ReadFileIntoBuffer("./idi.idi");
    ParseJson(Source);

#if BUILD_GUI
    DisplayWindow();
#endif
    FreeBuffer(Source);

    return Result;
}
