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

static s32 GetStringLength(char *String)
{
    s32 Result = -1;
    while(String[++Result]);
    return Result;
}

static char *StringCopy(char *String)
{
    s32 StringLength = GetStringLength(String);
    s32 StringSize = sizeof(char) * StringLength;
    char *Result = malloc(StringSize);
    memcpy(Result, String, StringSize);
    return Result;
}

static s32 FtwFunc(const char *fpath, const struct stat *sb, int typeflag)
{
    static s32 i = 0;
    if(i < MAX_FILE_INFO_CACHE)
    {
        FILE_INFO_CACHE[i].fpath = StringCopy((char *)fpath);
        FILE_INFO_CACHE[i].size = sb->st_size;
        FILE_INFO_CACHE[i].typeflag = typeflag;
    }
    else
    {
        printf("MAX_FILE_INFO_CACHE reached\n");
    }
    ++i;
    return 0;
}

file_info *FileTreeWalk(char *Path)
{
    s32 FtwResult = ftw(Path, FtwFunc, 1);
    s32 I = 0;
    while(FILE_INFO_CACHE[++I].fpath != 0);
    size FileInfoSize = I * sizeof(file_info);
    file_info *Result = malloc(FileInfoSize);
    memcpy(Result, FILE_INFO_CACHE, FileInfoSize);
    memset(FILE_INFO_CACHE, 0, FileInfoSize);
    if(FtwResult)
    {
        printf("ftw error\n");
    }
    return Result;
}

char *ResolvePath(char *BasePath, char *Path)
{
    s32 PathI, I = 0, State = 0;
    s32 BasePathLength = GetStringLength(BasePath);
    s32 PathLength = GetStringLength(Path);
    u32 Offset = 1;
    while(State >= 0 && Path[I])
    {
        switch(State)
        {
        case 0:
            State = Path[I++] == '.' ? 1 : -2;
            break;
        case 1:
            if(Path[I] == '.')
            {
                State = 2;
                ++I;
            }
            else if(Path[I] == '/')
            {
                State = -1;
                ++I;
            }
            else
            {
                State = 0;
                ++I;
            }
            break;
        case 2:
            ++Offset;
            if(Path[I] == '/')
            {
                ++I;
                State = 3;
            }
            else
            {
                State = -1;
            }
            break;
        case 3:
            if(Path[I] == '.')
            {
                ++I;
                State = 4;
            }
            else
            {
                State = -1;
            }
            break;
        case 4:
            if(Path[I] == '.')
            {
                ++I;
                State = 2;
            }
            else
            {
                State = -1;
            }
            break;
        default:
            State = -1;
            break;
        }
    }
    if(State == -2)
    {
        return Path;
    }
    PathI = I;
    I = BasePathLength - 1;
    while(Offset > 0)
    {
        if(BasePath[I] == '/')
        {
            ++I;
            --Offset;
        }
        else
        {
            --I;
        }
    }
    u32 WholePathSize = I + (PathLength - PathI);
    char *Result = malloc(WholePathSize + 1);
    Result[WholePathSize] = 0;
    memcpy(Result, BasePath, I);
    memcpy(Result + I, Path + PathI, PathLength - PathI);
    return Result;
}
