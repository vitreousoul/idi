typedef struct VecHdr {
    size_t len;
    size_t cap;
    char vec[];
} VecHdr;

#define vec__hdr(b) ((VecHdr *)((char *)(b) - offsetof(VecHdr, vec)))

#define vec_len(b) ((b) ? vec__hdr(b)->len : 0)
#define vec_cap(b) ((b) ? vec__hdr(b)->cap : 0)
#define vec_end(b) ((b) + vec_len(b))
#define vec_sizeof(b) ((b) ? vec_len(b)*sizeof(*b) : 0)

#define vec_free(b) ((b) ? (free(vec__hdr(b)), (b) = NULL) : 0)
#define vec_fit(b, n) ((n) <= vec_cap(b) ? 0 : ((b) = vec__grow((b), (n), sizeof(*(b)))))
#define vec_push(b, ...) (vec_fit((b), 1 + vec_len(b)), (b)[vec__hdr(b)->len++] = (__VA_ARGS__))
#define vec_clear(b) ((b) ? vec__hdr(b)->len = 0 : 0)

static void *vec__grow(const void *vec, size_t new_len, size_t elem_size)
{
    assert(vec_cap(vec) <= (SIZE_MAX - 1)/2);
    size_t new_cap = CLAMP_MIN(2*vec_cap(vec), MAX(new_len, 16));
    assert(new_len <= new_cap);
    assert(new_cap <= (SIZE_MAX - offsetof(VecHdr, vec))/elem_size);
    size_t new_size = offsetof(VecHdr, vec) + new_cap*elem_size;
    VecHdr *new_hdr;
    if (vec)
    {
        new_hdr = realloc(vec__hdr(vec), new_size);
    }
    else
    {
        new_hdr = malloc(new_size);
        new_hdr->len = 0;
    }
    new_hdr->cap = new_cap;
    return new_hdr->vec;
}

s32 vec_test(void);
s32 vec_test()
{
    printf("\nvec_test:\n");
    s32 Result = 0;
    s32 *Values = 0;
    size I;
    vec_push(Values, 42);
    vec_push(Values, 23);
    vec_push(Values, -23);
    for(I = 0; I < vec_len(Values); I++)
    {
        printf("  %d\n", Values[I]);
    }
    vec_free(Values);
    return Result;
}
