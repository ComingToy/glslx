#ifndef __GLSLD_COMPLETION_H__
#define __GLSLD_COMPLETION_H__

#include "doc.hpp"
#include <vector>
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
};

struct CompletionResultSet {
    std::vector<CompletionResult> types;
    std::vector<CompletionResult> funcs;
    std::vector<CompletionResult> variables;
    std::vector<CompletionResult> keywords;
    std::vector<CompletionResult> builtins;
};

extern void completion(Doc& doc, std::string const& input, const int line, const int col, CompletionResultSet& results);
#endif
