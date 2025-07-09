#include "document_symbol.hpp"
#include "doc.hpp"
#include <vector>

std::vector<DocumentSymbol> document_symbol(Doc* doc)
{
    if (!doc)
        return {};

    const auto& userdef_types = doc->userdef_types();
    const auto& globals = doc->globals();
    const auto& funcs = doc->func_defs();

    std::vector<DocumentSymbol> symbols;

    DocumentSymbol symbol;
    for (const auto& s : userdef_types) {
        const auto& loc = s->getLoc();
        if (doc->uri() != loc.getFilename()) {
            continue;
        }

        const auto& ty = s->getType();
        const auto* tyname = ty.getTypeName().c_str();
        const auto& detail = ty.getCompleteString(true, false, false);
        if (!ty.isStruct()) {
            continue;
        }

        Range range;
        range.start.line = loc.line;
        range.start.character = loc.column;
        range.end = range.start;

        DocumentSymbol symbol = {tyname, "struct", SymbolKind::Struct, range, range};

        auto const& members = *ty.getStruct();
        for (int i = 0; i < members.size(); ++i) {
            const glslang::TType* field = members[i].type;
            const auto* fieldname = field->getFieldName().c_str();
            auto detail = field->getCompleteString(true, false, false);
            symbol.children.push_back({fieldname, detail.c_str(), SymbolKind::Field});
        }

        symbols.emplace_back(std::move(symbol));
    }

    return symbols;
}
