#include "document_symbol.hpp"
#include "doc.hpp"
#include <vector>

static void document_userdef_types(Doc* doc, std::vector<DocumentSymbol>& symbols)
{
    const auto& userdef_types = doc->userdef_types();
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
        range.start.line = loc.line - 1;
        range.start.character = loc.column - 1;
        range.end = range.start;

        DocumentSymbol symbol = {tyname, "struct", SymbolKind::Struct, range, range};

        auto const& members = *ty.getStruct();
        for (int i = 0; i < members.size(); ++i) {
            const glslang::TType* field = members[i].type;
            const auto loc = members[i].loc;
            const auto* fieldname = field->getFieldName().c_str();

            Range range;
            range.start.line = loc.line - 1;
            range.start.character = loc.column - 1;
            range.end = range.start;
            auto detail = field->getCompleteString(true, false, false);
            symbol.children.push_back({fieldname, detail.c_str(), SymbolKind::Field, range, range});
        }

        symbols.emplace_back(std::move(symbol));
    }
}

static void document_globals(Doc* doc, std::vector<DocumentSymbol>& symbols)
{
    const auto& globals = doc->globals();
    for (const auto& g : globals) {
        const auto* name = g->getName().c_str();
        std::string detail = g->getType().getCompleteString(true, false, false).c_str();
        auto loc = g->getLoc();
        Range range;
        range.start.line = loc.line - 1;
        range.start.character = loc.column - 1;
        range.end = range.start;

        symbols.push_back({name, detail.c_str(), SymbolKind::Variable, range, range});
    }
}

std::vector<DocumentSymbol> document_symbol(Doc* doc)
{
    if (!doc)
        return {};

    const auto& funcs = doc->func_defs();

    std::vector<DocumentSymbol> symbols;
    document_userdef_types(doc, symbols);
    document_globals(doc, symbols);

    return symbols;
}
