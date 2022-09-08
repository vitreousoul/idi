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

    if (Begin >= 0 && End <= Buffer->Size) {
        for (size Index = Begin; Index < End; Index++)
        {
            Result->Data[Index - Begin] = Buffer->Data[Index];
            /* printf("Buffer Char Copy: %c\n", Result->Data[Index]); */
        }
    }

    return Result;
}

static size
NullTerminatedStringLength(const char *String)
{
    size Result = 0;

    while(String[Result] != '\0')
    {
        Result++;
    }

    return Result;
}

buffer *
BufferFromNullTerminatedString(const char *String)
{
    size StringLength = NullTerminatedStringLength(String);
    buffer *Result = CreateBuffer(StringLength);

    for (size Index = 0; String[Index] != '\0'; Index++)
    {
        Result->Data[Index] = String[Index];
    }

    return Result;
}

void
DebugPrintBuffer(buffer *Buffer)
{
    for(size Index = 0; Index < Buffer->Size; Index++)
    {
        printf("%c", Buffer->Data[Index]);
    }
}
