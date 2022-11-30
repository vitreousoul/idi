struct buffer {
    size Size;
    u8 *Data;
};
typedef struct buffer buffer;

buffer *CreateBuffer(size Size);
void FreeBuffer(buffer *Buffer);
buffer *BufferFromNullTerminatedString(char *String);
void DebugPrintBuffer(buffer *Buffer);
