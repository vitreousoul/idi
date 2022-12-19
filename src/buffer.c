buffer *CreateBuffer(size Size)
{
    buffer *Result = malloc(sizeof(buffer));
    Result->Size = Size;
    u8 *Data = malloc(Size * sizeof(u8));
    Result->Data = Data;

    return Result;
}

void FreeBuffer(buffer *Buffer)
{
    free(Buffer->Data);
    free(Buffer);
}
