// ==========
//   Grammar
// ==========
//
// TopLevel = Statement*
//
// TODO: some things like function declarations should not end in /;/
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
