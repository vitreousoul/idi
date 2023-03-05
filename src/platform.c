#define MAX_FILE_INFO_CACHE 1024
file_info FILE_INFO_CACHE[MAX_FILE_INFO_CACHE];

size GetFileSize(FILE *File)
{
    size Result;

    fseek(File, 0, SEEK_END);
    Result = ftell(File);

    return(Result);
}

buffer *ReadFileIntoBuffer(char *FilePath)
{
    FILE *File = fopen(FilePath, "rb");
    size FileSize = GetFileSize(File);
    buffer *Result = CreateBuffer(FileSize);

    fseek(File, 0, SEEK_SET);
    fread(Result->Data, sizeof(*Result->Data), FileSize, File);
    fclose(File);

    return Result;
}

void WriteBufferToFile(buffer *Buffer, char *FilePath)
{
    FILE *File = fopen(FilePath, "wb");

    fwrite(Buffer->Data, sizeof(*Buffer->Data), Buffer->Size, File);
    fclose(File);
}

void PrintLog(char *Type, char *Message)
{
    printf("[ %s ] %s\n", Type, Message);
}

void PrintError(char *Message)
{
    PrintLog("Error", Message);
}

s32 FtwFunc(const char *fpath, const struct stat *sb, int typeflag)
{
    printf("ftw %s\n", fpath);
    return 0;
}

file_info *FileTreeWalk(char *Path)
{
    s32 FtwResult = ftw(Path, FtwFunc, 1);
    return FILE_INFO_CACHE;
}
