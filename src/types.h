#ifndef TYPES_H_
#define TYPES_H_

#include <inttypes.h>
#include <stdlib.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint32_t b32;
typedef size_t size;

struct buffer {
    size Size;
    u8 *Data;
};
typedef struct buffer buffer;

#endif // TYPES_H_
