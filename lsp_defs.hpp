#ifndef __GLSLD_LSP_DEFS_HPP__
#define __GLSLD_LSP_DEFS_HPP__

#include "nlohmann/json.hpp"
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

struct Position {
    int line;
    int character;

    inline nlohmann::json json() const { return nlohmann::json{{"line", line}, {"character", character}}; };
};

struct Range {
    Position start;
    Position end;

    inline nlohmann::json json() const
    {
        nlohmann::json range;
        range["start"] = start.json();
        range["end"] = end.json();
        return range;
    }
};

struct DocumentSymbol {
    std::string name;
    std::string detail;
    SymbolKind kind;
    Range range;
    Range selectionRange;
    std::vector<DocumentSymbol> children;

    inline nlohmann::json json() const
    {
        nlohmann::json symbol;
        symbol["name"] = name;
        symbol["detail"] = detail;
        symbol["kind"] = int(kind);
        symbol["range"] = range.json();
        symbol["selectionRange"] = selectionRange.json();

        for (auto const& child : children) {
            symbol["children"].push_back(child.json());
        }

        return symbol;
    }
};

#endif
