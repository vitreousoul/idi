#ifndef IDI_H_
#define IDI_H_

#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

typedef uint8_t u8;
typedef size_t size;

struct buffer {
    size Size;
    u8 *Data;
};
typedef struct buffer buffer;

#endif // IDI_H_
