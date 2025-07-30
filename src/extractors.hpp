#ifndef __GLSLX_EXTRACTORS_HPP__
#define __GLSLX_EXTRACTORS_HPP__
#include "glslang/Include/intermediate.h"
#include <iostream>

struct LocalDefUseExtractor : public glslang::TIntermTraverser {
public:
    glslang::TSourceLoc end_loc;
    std::vector<glslang::TIntermSymbol*> defs, uses;
    std::map<int, std::vector<TIntermNode*>> nodes_by_line;
    std::vector<glslang::TIntermSymbol*> userdef_types;

    void visitConstantUnion(glslang::TIntermConstantUnion* node) override
    {
        auto loc = node->getLoc();
        if (loc.line > end_loc.line) {
            end_loc = loc;
        } else if (loc.line == end_loc.line && loc.column > end_loc.column) {
            end_loc = loc;
        }
        nodes_by_line[node->getLoc().line].push_back(node);
    }
    bool visitBinary(glslang::TVisit, glslang::TIntermBinary* node) override
    {
        auto loc = node->getLoc();
        if (loc.line > end_loc.line) {
            end_loc = loc;
        } else if (loc.line == end_loc.line && loc.column > end_loc.column) {
            end_loc = loc;
        }

        nodes_by_line[node->getLoc().line].push_back(node);
        return true;
    }
    bool visitSelection(glslang::TVisit, glslang::TIntermSelection* node) override
    {
        auto loc = node->getLoc();
        if (loc.line > end_loc.line) {
            end_loc = loc;
        } else if (loc.line == end_loc.line && loc.column > end_loc.column) {
            end_loc = loc;
        }

        nodes_by_line[node->getLoc().line].push_back(node);
        return true;
    }
    bool visitAggregate(glslang::TVisit, glslang::TIntermAggregate* node) override
    {
        auto loc = node->getLoc();
        if (loc.line > end_loc.line) {
            end_loc = loc;
        } else if (loc.line == end_loc.line && loc.column > end_loc.column) {
            end_loc = loc;
        }

        nodes_by_line[node->getLoc().line].push_back(node);
        return true;
    }
    bool visitLoop(glslang::TVisit, glslang::TIntermLoop* node) override
    {
        auto loc = node->getLoc();
        if (loc.line > end_loc.line) {
            end_loc = loc;
        } else if (loc.line == end_loc.line && loc.column > end_loc.column) {
            end_loc = loc;
        }

        nodes_by_line[node->getLoc().line].push_back(node);
        return true;
    }
    bool visitBranch(glslang::TVisit, glslang::TIntermBranch* node) override
    {
        auto loc = node->getLoc();
        if (loc.line > end_loc.line) {
            end_loc = loc;
        } else if (loc.line == end_loc.line && loc.column > end_loc.column) {
            end_loc = loc;
        }

        nodes_by_line[node->getLoc().line].push_back(node);
        return true;
    }
    bool visitSwitch(glslang::TVisit, glslang::TIntermSwitch* node) override
    {
        auto loc = node->getLoc();
        if (loc.line > end_loc.line) {
            end_loc = loc;
        } else if (loc.line == end_loc.line && loc.column > end_loc.column) {
            end_loc = loc;
        }

        nodes_by_line[node->getLoc().line].push_back(node);
        return true;
    }

    void visitSymbol(glslang::TIntermSymbol* symbol) override
    {
        auto loc = symbol->getLoc();
        if (loc.line > end_loc.line) {
            end_loc = loc;
        } else if (loc.line == end_loc.line && loc.column > end_loc.column) {
            end_loc = loc;
        }

        nodes_by_line[symbol->getLoc().line].push_back(symbol);
        uses.push_back(symbol);
    }
    bool visitUnary(glslang::TVisit v, glslang::TIntermUnary* unary) override
    {
        (void)v;
        nodes_by_line[unary->getLoc().line].push_back(unary);
        auto loc = unary->getLoc();
        if (loc.line > end_loc.line) {
            end_loc = loc;
        } else if (loc.line == end_loc.line && loc.column > end_loc.column) {
            end_loc = loc;
        }

        if (unary->getOp() == glslang::EOpDeclareType) {
            auto* sym = unary->getOperand()->getAsSymbolNode();
            if (sym)
                userdef_types.push_back(sym);
            return false;
        } else if (unary->getOp() == glslang::EOpDeclare) {
            defs.push_back(unary->getOperand()->getAsSymbolNode());
            return false;
        } else {
            return true;
        }
    }
};

class DocInfoExtractor : public glslang::TIntermTraverser {
public:
    struct FunctionDefDesc {
        glslang::TIntermAggregate* def;
        std::vector<glslang::TIntermSymbol*> args;
        std::vector<glslang::TIntermSymbol*> local_defs;
        std::vector<glslang::TIntermSymbol*> local_uses;
        std::vector<glslang::TIntermSymbol*> userdef_types;
        glslang::TSourceLoc start, end;
    };

    std::map<int, std::vector<TIntermNode*>> nodes_by_line;
    std::vector<glslang::TIntermSymbol*> uses;
    std::vector<glslang::TIntermSymbol*> globals;
    std::vector<FunctionDefDesc> funcs;
    std::vector<glslang::TIntermSymbol*> userdef_types;
    bool visitBinary(glslang::TVisit, glslang::TIntermBinary* node) override
    {
        if (node->getOp() == glslang::EOpIndexDirectStruct || node->getOp() == glslang::EOpIndexDirect ||
            node->getOp() == glslang::EOpIndexIndirect) {
            nodes_by_line[node->getLoc().line].push_back(node);
        }
        return true;
    }

    bool visitAggregate(glslang::TVisit, glslang::TIntermAggregate* agg) override
    {
        if (agg->getOp() == glslang::EOpLinkerObjects) {
            for (auto& obj : agg->getSequence()) {
                auto sym = obj->getAsSymbolNode();
                if (!sym)
                    continue;
                auto loc = sym->getLoc();
                if (!loc.name) // builtin
                    continue;
                globals.push_back(sym);
            }
            return false;
        }

        if (agg->getOp() == glslang::EOpFunction) {

            struct FunctionDefDesc function_def = {agg, {}, {}, {}, {}, agg->getLoc(), agg->getEndLoc()};

            auto& children = agg->getSequence();
            if (children.size() != 2) {
                std::cerr << "found func " << agg->getName() << " but children size != 2" << std::endl;
                return true;
            }

            std::vector<glslang::TIntermSymbol*> args;
            auto* params = children[0]->getAsAggregate();
            if (!params || params->getOp() != glslang::EOpParameters) {
                std::cerr << "found func " << agg->getName() << " but children[0].op != EOpParameters" << std::endl;
                return true;
            }

            for (auto* arg : params->getSequence()) {
                if (arg->getAsSymbolNode())
                    function_def.args.push_back(arg->getAsSymbolNode());
            }

            auto* body = children[1];
            LocalDefUseExtractor extractor;
            body->traverse(&extractor);
            function_def.local_defs.swap(extractor.defs);
            function_def.local_uses.swap(extractor.uses);
            function_def.userdef_types.swap(extractor.userdef_types);

            std::cerr << "found function def " << agg->getName() << " at " << agg->getLoc().getFilename() << ":"
                      << agg->getLoc().line << ":" << agg->getLoc().column << " to "
                      << body->getAsAggregate()->getEndLoc().line
                      << " return type: " << agg->getType().getCompleteString() << " has " << agg->getSequence().size()
                      << " sub nodes" << std::endl;

            function_def.end = body->getAsAggregate()->getEndLoc();
            funcs.emplace_back(std::move(function_def));
            for (auto [line, node] : extractor.nodes_by_line) {
                nodes_by_line[line] = node;
            }
            return false;
        }

        return true;
    }

    void visitConstantUnion(glslang::TIntermConstantUnion* node) override {}
    bool visitUnary(glslang::TVisit v, glslang::TIntermUnary* unary) override
    {
        if (unary->getOp() == glslang::EOpDeclareType) {
            auto* sym = unary->getOperand()->getAsSymbolNode();
            if (sym)
                userdef_types.push_back(sym);
            return false;
        }

        return true;
    }
};

#endif
