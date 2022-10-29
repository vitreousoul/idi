#include "idi.h"

#include "buffer.c"
#include "platform.c"
#include "parser.c"
#include "json_parser.c"

int main()
{
    int Result = 0;
    buffer *Source = BufferFromNullTerminatedString("[ 123, 432 ]");
    json_token_list *Tokens = ParseJson(Source);

    while(Tokens != 0)
    {
        printf("token %s\n", GetJsonTokenTypeString(Tokens->Type));
        Tokens = Tokens->Next;
    }

    return Result;
}
