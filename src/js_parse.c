// ==========
//   Grammar
// ==========
//
// NOTE: we are starting off with module declarations so we don't have to tackle the entire
// grammar at once. Also, modules are usually at the top of the file, so we can parse the
// beginning of some set of files and display dependencies.
//
// TopLevel = Declaration*
// Identifier = /[_a-zA-Z]/ /[_a-zA-Z0-9]/*
// String = ...
// Declaration = Module
// Module = Import | Export
// Import = /import/ ModuleType? ModulePath /;/
// Export = /export/ ModuleType? ModulePath /;/
// ModuleType = ModuleParams /from/
// ModuleParams = Identifier | ModuleParamObject
// ModulePath = String
// ModuleParamObject = /{/ ModuleParam* /}/
// ModuleParamAttribute = ModuleParam ModuleParamAttributeNext*
// ModuleParamAttributeNext = /,/ ModuleParam
// ModuleParam = Identifier ModuleParamAlias?
// ModuleParamAlias = /as/ Identifier
//

static void ParseJs(buffer Source, token *Tokens, u32 TokenCount)
{
    s32 I;
    for(I = 0; I < TokenCount; ++I)
    {
        switch(Tokens[I].Kind)
        {
            case token_kind_Identifier:
                printf("ident ");
                break;
            default:
                printf("(error)");
                break;
        }
    }
}
