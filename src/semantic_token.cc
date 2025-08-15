#include "semantic_token.hpp"
#include "doc.hpp"
#include <algorithm>
#include <cstdio>
#include <vector>

nlohmann::json semantic_token(Doc* doc)
{
    /*
	 * "tokenTypes": ["type", "struct", "parameter", "variable", "function", "keyword", "macro", "modifier", "number", "operator", "comment"],
	 * "tokenModifiers": ["declaration", "definition", "readonly", "static"]
	 * */
    struct Token {
        int line;
        int col;
        int length;
        int type;
        int modifier;
    };

    std::vector<Token> tokens;
    nlohmann::json result;

    if (!doc || doc->inactive_blocks().empty()) {
        return result;
    }
#if 0
    for (auto* sym : doc->globals()) {
        const auto& name = sym->getName();
        auto loc = sym->getLoc();
        if (name.substr(0, 5) == "anon@") {
            continue;
        }
        tokens.push_back({loc.line - 1, loc.column - 1, (int)name.size(), 3, 1});
    }

    for (const auto& func : doc->func_defs()) {
        auto loc = func.def->getLoc();
        auto const& name = func.def->getName();
        tokens.push_back({loc.line - 1, loc.column - 1, (int)name.size(), 4, 1});

        for (auto* sym : func.local_defs) {
            const auto& name = sym->getName();
            auto loc = sym->getLoc();
            if (name.substr(0, 5) == "anon@") {
                continue;
            }
            tokens.push_back({loc.line - 1, loc.column - 1, (int)name.size(), 3, 1});
        }

        for (auto* sym : func.local_uses) {
            const auto& name = sym->getName();
            auto loc = sym->getLoc();
            if (name.substr(0, 5) == "anon@") {
                continue;
            }
            tokens.push_back({loc.line - 1, loc.column - 1, (int)name.size(), 3, 0});
        }
    }
#endif
    auto const& lines = doc->lines();
    for (auto const& block : doc->inactive_blocks()) {
        for (int i = block.start; i <= block.end; ++i) {
            tokens.push_back({i, 0, (int)lines[i].size(), 10, 0});
        }
    }

    std::sort(tokens.begin(), tokens.end(), [](Token const& lhs, Token const& rhs) {
        if (lhs.line < rhs.line) {
            return true;
        }

        if (lhs.line > rhs.line) {
            return false;
        }

        return lhs.col < rhs.col;
    });

    if (tokens.empty()) {
        return result;
    }

    result.push_back(tokens[0].line);
    result.push_back(tokens[0].col);
    result.push_back(tokens[0].length);
    result.push_back(tokens[0].type);
    result.push_back(tokens[0].modifier);

    for (int i = 1; i < tokens.size(); ++i) {
        int line = tokens[i].line - tokens[i - 1].line;
        int col = line == 0 ? tokens[i].col - tokens[i - 1].col : tokens[i].col;

        result.push_back(line);
        result.push_back(col);
        result.push_back(tokens[i].length);
        result.push_back(tokens[i].type);
        result.push_back(tokens[i].modifier);
    }

    return result;
}
