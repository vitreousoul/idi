#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdio.h>

#include "types.h"

struct buffer {
    size Size;
    u8 *Data;
};
typedef struct buffer buffer;

buffer *CreateBuffer(size Size);
buffer *BufferFromNullTerminatedString(char *String);
buffer *GetBufferSubRegion(buffer *Buffer, size Begin, size End);
void DebugPrintBuffer(buffer *Buffer);

#endif // BUFFER_H_
