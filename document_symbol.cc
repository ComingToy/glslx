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
}
