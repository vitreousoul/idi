// ==========
//   Grammar
// ==========
//
// TopLevel = Statement*
//
// Statement = StatementContent /;/
// StatementContent = Assignment | Module
// Declaration = Class | Function | NameSpace
//
// Module = Import | Export
//
// Import = /import/ ImportType ImportPath
// ImportType = ImportType /from/
// ImportPath = String
//

static void ParseJs(buffer Source, token *Tokens, u32 TokenCount)
{
    s32 I;
    hash_table IdentifierTable = CreateHashTable(2048);
    for(I = 0; I < TokenCount; ++I)
    {
        switch(Tokens[I].Kind)
        {
            case token_kind_Identifier:
                break;
            default:
                printf("(error)");
                break;
        }
    }
}
