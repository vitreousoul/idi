#define IS_POW2(x) (((x) != 0) && ((x) & ((x)-1)) == 0)

static b32 StringMatch(char *A, char *B)
{
    b32 Result = True;
    while(*A != 0 && *B != 0)
    {
        if(*A++ != *B++)
        {
            Result = False;
            break;
        }
    }
    return Result;
}

static u64 StringHash(char *String)
{
    u64 Result = 5381;
    u8 Char;
    while((Char = *String++))
    {
        Result ^= Char * Result;
    }
    return Result;
}

static u64 HashTableGetIndex(hash_table *HashTable, char *Key)
{
    u64 Result = StringHash(Key);
    for(;;)
    {
        Result &= HashTable->Capacity - 1;
        if(!HashTable->Items[Result].Key)
        {
            break;
        }
        else if(StringMatch(HashTable->Items[Result].Key, Key))
        {
            break;
        }
        ++Result;
    }
    return Result;
}

static u64 HashTableGet(hash_table *HashTable, char *Key)
{
    u32 HashTableIndex = HashTableGetIndex(HashTable, Key);
    u32 Result = HashTable->Items[HashTableIndex].Value;
    return Result;
}

static void HashTableSet(hash_table *HashTable, char *Key, u64 Value)
{
    if(HashTable->Count >= HashTable->Capacity)
    {
        printf("Error: HashTable full");
        return;
    }
    assert(IS_POW2(HashTable->Capacity));
    u32 HashTableIndex = HashTableGetIndex(HashTable, Key);
    HashTable->Items[HashTableIndex].Key = Key;
    HashTable->Items[HashTableIndex].Value = Value;
}

static hash_table CreateHashTable(u32 Count)
{
    u32 I;
    hash_table HashTable;
    hash_table_item *Items = malloc(sizeof(hash_table_item)*Count);
    HashTable.Items = Items;
    HashTable.Capacity = Count;
    HashTable.Count = 0;
    memset(Items, 0, sizeof(hash_table_item)*Count);
    return HashTable;
}

result TestHashTable()
{
    s32 I, KeywordCount = 5;
    char *Strings[] = {"apple", "farm", "foo","bar", "fon","fop","foreach"};
    result Result = result_Ok;
    u32 ItemCount = 1 << 12;
    hash_table HashTable = CreateHashTable(ItemCount);
    for(I = 0; I < KeywordCount; ++I)
    {
        printf("Set hash-table '%s' %d\n", Strings[I], I*4+3);
        HashTableSet(&HashTable, Strings[I], I*4+3);
    }
    for(I = 0; I < KeywordCount; ++I)
    {
        u64 Value = HashTableGet(&HashTable, Strings[I]);
        printf("Get hash-table '%s' %llu\n", Strings[I], Value);
    }
    return Result;
}
