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
    buffer *Buffer = ReadFileIntoBuffer("idi.idi");

    parse_tree ParseTree = ParseBuffer(Buffer);
    int Result = GetParseTreeState(&ParseTree);
    printf("Parse Result: %s\n", DisplayParseTreeState(&ParseTree));

    free(Buffer->Data);
    free(Buffer);

    return Result;
}
