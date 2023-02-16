typedef struct
{
    char *Key;
    u64 Value;
} hash_table_item;

typedef struct
{
    hash_table_item *Items;
    u32 Capacity;
    u32 Count;
} hash_table;

result TestHashTable(void);
