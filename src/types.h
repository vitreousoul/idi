typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

typedef float f32;
typedef double f64;

typedef uint32_t b32;
enum b32 {
False = (u32)0,
True = (u32)1,
};

typedef size_t size;

typedef enum
{
    result_Ok = 0,
    result_Error = 1,
} result;

#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define CLAMP_MAX(x, max) MIN(x, max)
#define CLAMP_MIN(x, min) MAX(x, min)
