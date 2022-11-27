struct buffer {
    size Size;
    u8 *Data;
};
typedef struct buffer buffer;

buffer *CreateBuffer(size Size);
void FreeBuffer(buffer *Buffer);
buffer *BufferFromNullTerminatedString(char *String);
buffer *GetBufferSubRegion(buffer *Buffer, size Begin, size End);
void DebugPrintBuffer(buffer *Buffer);
