#include "idi.h"

static size
GetFileSize(FILE *File)
{
    size Result;

    fseek(File, 0, SEEK_END);
    Result = ftell(File);

    return(Result);
}

static buffer *
ReadFileIntoBuffer(const char *FilePath)
{
    FILE *File = fopen(FilePath, "rb");
    size FileSize = GetFileSize(File);
    buffer *Result = CreateBuffer(FileSize);

    fseek(File, 0, SEEK_SET);
    fread(Result->Data, sizeof(*Result->Data), FileSize, File);
    fclose(File);

    return Result;
}

static void
WriteBufferToFile(buffer *Buffer, const char *FilePath)
{
    FILE *File = fopen(FilePath, "wb");

    fwrite(Buffer->Data, sizeof(*Buffer->Data), Buffer->Size, File);
    fclose(File);
}

int
main()
{
    int Result = 0;
    buffer *Buffer = ReadFileIntoBuffer("idi.idi");

    token_list *ParseResult = Parse(*Buffer);

    while(ParseResult != NULL)
    {
        printf("Token: %d\n", ParseResult->Type);
        printf("    ");
        DebugPrintBuffer(ParseResult->Text);
        printf("\n");
        ParseResult = ParseResult->Next;
    }
    /* WriteBufferToFile(Buffer, "test.idi"); */

    Result = Buffer->Size;

    free(Buffer->Data);
    free(Buffer);
    // TODO: free token_list!!!!

    return Result;
}
