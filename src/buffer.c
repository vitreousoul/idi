buffer *
CreateBuffer(size Size)
{
    buffer *Result = malloc(sizeof(buffer));
    Result->Size = Size;
    u8 *Data = malloc(Size * sizeof(u8));
    Result->Data = Data;

    return Result;
}

buffer *
GetBufferSubRegion(buffer *Buffer, size Begin, size End)
{
    buffer *Result = CreateBuffer(End - Begin);
    size Index;

    if (Begin >= 0 && End <= Buffer->Size) {
        for (Index = Begin; Index < End; Index++)
        {
            Result->Data[Index - Begin] = Buffer->Data[Index];
        }
    }

    return Result;
}

static size
NullTerminatedStringLength(char *String)
{
    size Result = 0;

    while(String[Result] != '\0')
    {
        Result++;
    }

    return Result;
}

buffer *
BufferFromNullTerminatedString(char *String)
{
    size StringLength = NullTerminatedStringLength(String);
    buffer *Result = CreateBuffer(StringLength);
    size Index;

    for (Index = 0; String[Index] != '\0'; Index++)
    {
        Result->Data[Index] = String[Index];
    }

    return Result;
}

void
DebugPrintBuffer(buffer *Buffer)
{
    size Index;

    for(Index = 0; Index < Buffer->Size; Index++)
    {
        printf("%c", Buffer->Data[Index]);
    }
}
