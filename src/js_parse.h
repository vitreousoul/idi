typedef struct
{
    u8 *Name;
    // TODO: alias, destructor
} js_ast_module_param;

typedef enum
{
    Import,
    Export,
} js_ast_module_kind;

typedef struct
{
    js_ast_module_kind Kind;
    u32 ParamCount;
    vec_ptr(js_ast_module_param *Params);
    u8 *Path;
} js_ast_module;

typedef enum
{
    js_ast_kind_Module,
} js_ast_kind;

typedef struct
{

} js_ast;

typedef struct
{

} js_parser;
