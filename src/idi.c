#include "idi.h"

#include "buffer.c"
#include "platform.c"
#include "json_parser.c"
#include "gui.c"

int main()
{
    int Result = 0;
    /* TODO: read from file so we can test parsing strings without have to remove escapes */
    buffer *Source = ReadFileIntoBuffer("./idi.idi");
    /* buffer *Source = BufferFromNullTerminatedString("{ 123, \"foo\": true }"); */
    ParseJson(Source);

    DisplayWindow();

    return(Result);
}
