size
GetFileSize(FILE *File)
{
    size Result;

    fseek(File, 0, SEEK_END);
    Result = ftell(File);

    return(Result);
}

buffer *
ReadFileIntoBuffer(char *FilePath)
{
    FILE *File = fopen(FilePath, "rb");
    size FileSize = GetFileSize(File);
    buffer *Result = CreateBuffer(FileSize);

    fseek(File, 0, SEEK_SET);
    fread(Result->Data, sizeof(*Result->Data), FileSize, File);
    fclose(File);

    return Result;
}

void
WriteBufferToFile(buffer *Buffer, char *FilePath)
{
    FILE *File = fopen(FilePath, "wb");

    fwrite(Buffer->Data, sizeof(*Buffer->Data), Buffer->Size, File);
    fclose(File);
}

void
PrintLog(char *Type, char *Message)
{
    printf("[ %s ] %s\n", Type, Message);
}

void
PrintMessage(char *Message)
{
    PrintLog("Log", Message);
}

void
PrintError(char *Message)
{
    PrintLog("Error", Message);
}
