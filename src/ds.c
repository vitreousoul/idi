#define IS_POW2(x) (((x) != 0) && ((x) & ((x)-1)) == 0)

static u64 StringHash(char *String)
{
    u64 Result = 5381;
    u8 Char;
    while((Char = *String++))
    {
        Result = ((Result << 5) + Result) + Char;
    }
    return Result;
}

static u64 HashTableGet(hash_table *HashTable, char *Key)
{
    u32 Result = 0;
    u64 HashValue = StringHash(Key);
    for(;;)
    {
        HashValue &= HashTable->Capacity - 1;
        if(!HashTable->Items[HashValue].Key)
        {
            break;
        }
        else if(strcmp(HashTable->Items[HashValue].Key, Key))
        {
            Result = HashTable->Items[HashValue].Value;
            break;
        }
        ++HashValue;
    }
    return Result;
}

static result HashTableSet(hash_table *HashTable, char *Key, u64 Value)
{
    result Result = result_Ok;
    if(HashTable->Count >= HashTable->Capacity)
    {
        printf("Error: HashTable full");
        return result_Error;
    }
    assert(IS_POW2(HashTable->Capacity));
    u64 HashValue = StringHash(Key);
    for(;;)
    {
        HashValue &= HashTable->Capacity - 1;
        if(!HashTable->Items[HashValue].Key)
        {
            HashTable->Items[HashValue].Key = Key;
            HashTable->Items[HashValue].Value = Value;
            ++HashTable->Count;
            break;
        }
        else if(strcmp(HashTable->Items[HashValue].Key, Key))
        {
            HashTable->Items[HashValue].Value = Value;
            break;
        }
        ++HashValue;
    }
    return Result;
}

result TestHashTable()
{
    s32 I, KeywordCount = 2;
    char *Strings[] = {"foo","bar"};
    result Result = result_Ok;
    u32 ItemCount = 1 << 12;
    hash_table_item Items[ItemCount];
    memset(Items, 0, sizeof(hash_table_item)*ItemCount);
    hash_table HashTable;
    HashTable.Items = Items;
    HashTable.Capacity = ItemCount;
    HashTable.Count = 0;
    for(I = 0; I < KeywordCount; ++I)
    {
        printf("setting '%s'\n", Strings[I]);
        HashTableSet(&HashTable, Strings[I], I*4+2);
    }
    for(I = 0; I < KeywordCount; ++I)
    {
        u64 Value = HashTableGet(&HashTable, Strings[I]);
        printf("getting '%s' %llu\n", Strings[I], Value);
    }

    return Result;
}
