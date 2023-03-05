#include <ftw.h>

typedef struct
{
    char *fpath;
    struct stat *sb;
    int typeflag;
} file_info;

size GetFileSize(FILE *File);
buffer *ReadFileIntoBuffer(char *FilePath);
void WriteBufferToFile(buffer *Buffer, char *FilePath);
void PrintLog(char *Type, char *Message);
void PrintError(char *Message);
file_info *FileTreeWalk(char *Path);
