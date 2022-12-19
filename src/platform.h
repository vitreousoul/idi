size GetFileSize(FILE *File);
buffer *ReadFileIntoBuffer(char *FilePath);
void WriteBufferToFile(buffer *Buffer, char *FilePath);
void PrintLog(char *Type, char *Message);
void PrintError(char *Message);
