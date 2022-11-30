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
    /* TODO: read from file so we can test parsing strings without have to remove escapes */
    buffer *Source = ReadFileIntoBuffer("./idi.idi");
    ParseJson(Source);

#if BUILD_GUI
    DisplayWindow();
#endif
    FreeBuffer(Source);

    return(Result);
}
