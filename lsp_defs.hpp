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

enum class CompletionItemKind {
    Text = 1,
    Method = 2,
    Function = 3,
    Constructor = 4,
    Field = 5,
    Variable = 6,
    Class = 7,
    Interface = 8,
    Module = 9,
    Property = 10,
    Unit = 11,
    Value = 12,
    Enum = 13,
    Keyword = 14,
    Snippet = 15,
    Color = 16,
    File = 17,
    Reference = 18,
    Folder = 19,
    EnumMember = 20,
    Constant = 21,
    Struct = 22,
    Event = 23,
    Operator = 24,
    TypeParameter = 25,
};

enum class InsertTextFormat {
    /**
	 * The primary text to be inserted is treated as a plain string.
	 */
    PlainText = 1,

    /**
	 * The primary text to be inserted is treated as a snippet.
	 *
	 * A snippet can define tab stops and placeholders with `$1`, `$2`
	 * and `${3:foo}`. `$0` defines the final tab stop, it defaults to
	 * the end of the snippet. Placeholders with equal identifiers are linked,
	 * that is typing in one will update others too.
	 */
    Snippet = 2
};

struct CompletionResult {
    std::string label;
    CompletionItemKind kind;
    std::string detail;
    std::string documentation;
    std::string insert_text;
    InsertTextFormat insert_text_format;

    nlohmann::json json() const
    {
        nlohmann::json result;
        result["label"] = label;
        result["kind"] = int(kind);
        result["detail"] = detail;
        result["documentation"] = documentation;
        result["insertText"] = insert_text;
        result["insertTextFormat"] = insert_text_format;

        return result;
    }
};

struct CompletionResultSet {
    std::vector<CompletionResult> types;
    std::vector<CompletionResult> funcs;
    std::vector<CompletionResult> variables;
    std::vector<CompletionResult> keywords;
    std::vector<CompletionResult> builtins;
};

#endif
