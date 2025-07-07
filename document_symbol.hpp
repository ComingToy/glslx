#ifndef __GLSLD_DOCUMENT_SYMBOL_HPP__
#define __GLSLD_DOCUMENT_SYMBOL_HPP__

#include <string>
#include <vector>

enum class SymbolKind {
    File = 1,
    Module = 2,
    Namespace = 3,
    Package = 4,
    Class = 5,
    Method = 6,
    Property = 7,
    Field = 8,
    Constructor = 9,
    Enum = 10,
    Interface = 11,
    Function = 12,
    Variable = 13,
    Constant = 14,
    String = 15,
    Number = 16,
    Boolean = 17,
    Array = 18,
    Object = 19,
    Key = 20,
    Null = 21,
    EnumMember = 22,
    Struct = 23,
    Event = 24,
    Operator = 25,
    TypeParameter = 26,
};

struct DocumentSymbol {
    std::string name;
    std::string detail;
    SymbolKind kind;
    std::vector<DocumentSymbol> children;
};

std::vector<DocumentSymbol> document_symbol(std::string const& uri);
#endif


