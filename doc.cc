#include "doc.hpp"
#include "StandAlone/DirStackFileIncluder.h"
#include "args.hpp"
#include "glslang/Include/intermediate.h"
#include "glslang/MachineIndependent/SymbolTable.h"
#include "glslang/MachineIndependent/localintermediate.h"
#include "parser.hpp"
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <utility>
#include <vector>

extern int yylex(YYSTYPE*, glslang::TParseContext&);

Doc::Doc() { resource_ = nullptr; }

Doc::~Doc() { release_(); }

Doc::Doc(std::string const& uri, const int version, std::string const& text)
{
    resource_ = new __Resource;
    resource_->ref = 1;
    resource_->uri = uri;
    resource_->version = version;

    set_text(text);
    infer_language_();
    resource_->shader = nullptr;
}

void Doc::set_text(std::string const& text)
{
    if (!resource_)
        return;
    std::stringstream ss(text);
    std::string line;

    resource_->lines_.clear();
    while (std::getline(ss, line)) {
        resource_->lines_.emplace_back(std::move(line));
    }

    resource_->text_ = text;
}

Doc::Doc(const Doc& rhs)
{
    release_();
    resource_ = rhs.resource_;
    resource_->ref += 1;
}

Doc::Doc(Doc&& rhs)
{
    release_();
    resource_ = rhs.resource_;
    rhs.resource_ = nullptr;
}

Doc& Doc::operator=(const Doc& rhs)
{
    release_();
    resource_ = rhs.resource_;
    resource_->ref += 1;
    return *this;
}

Doc& Doc::operator=(Doc&& rhs)
{
    release_();
    resource_ = rhs.resource_;
    rhs.resource_ = nullptr;
    return *this;
}

void Doc::infer_language_()
{
    // support compute stage only now
    if (!resource_)
        return;

    std::filesystem::path p(uri());
    if (p.has_extension()) {
        auto extension = p.extension();
        auto s = extension.string();
        resource_->language = map_to_stage(s.substr(1));
    } else {
        resource_->language = EShLangCount;
    }
}

void Doc::release_()
{
    if (resource_) {
        if (resource_->ref == 1) {
            delete resource_;
        } else {
            resource_->ref -= 1;
        }
    }
}

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
    std::map<int, std::vector<TIntermNode*>> nodes_by_line;
    std::vector<glslang::TIntermSymbol*> uses;
    std::vector<glslang::TIntermSymbol*> globals;
    std::vector<Doc::FunctionDefDesc> funcs;
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

            struct Doc::FunctionDefDesc function_def = {agg, {}, {}, {}, {}, agg->getLoc(), agg->getEndLoc()};

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

class BuiltinSymbolTable : public glslang::TSymbolTable {
public:
    void get_all_symbols(std::vector<glslang::TSymbol*>& symbols)
    {
        for (int level = currentLevel(); level >= 0; --level) {
            glslang::TSymbolTableLevel* table_level = table[level];
            for (auto pos = table_level->get_level().begin(); pos != table_level->get_level().end(); ++pos) {
                symbols.push_back(pos->second);
            }
        }
    }
};

bool Doc::parse(CompileOption const& option)
{
    CompileOption compile_option = option;
    if (!resource_)
        return false;

    auto* resource = new Doc::__Resource;
    auto stage = compile_option.shader_stage == EShLangCount ? language() : compile_option.shader_stage;

    if (stage == EShLangCount) {
        std::cerr << "unkown stage: " << uri() << std::endl;
        return false;
    }
    resource->shader = std::make_unique<glslang::TShader>(stage);

    resource->nodes_by_line.clear();
    resource->globals.clear();
    resource->func_defs.clear();
    resource->userdef_types.clear();
    resource->builtins.clear();

    auto& shader = *resource->shader;
    shader.setDebugInfo(true);

    std::string preambles;
    for (auto const& [k, v] : compile_option.macros) {
        preambles.append("#define " + k + " " + v + "\n");
    }

    const std::string pound_extension = "#extension GL_GOOGLE_include_directive : enable\n";
    preambles += pound_extension;

    auto& shader_strings = resource_->text_;
    const char* shader_source = shader_strings.data();
    const int shader_lengths = (int)shader_strings.size();
    const char* string_names = resource_->uri.data();
    shader.setStringsWithLengthsAndNames(&shader_source, &shader_lengths, &string_names, 1);
    shader.setPreamble(preambles.c_str());
    shader.setEntryPoint(compile_option.entrypoint.c_str());

    shader.setAutoMapBindings(compile_option.auto_bind_uniforms);
    if (compile_option.auto_combinded_image_sampler) {
        shader.setTextureSamplerTransformMode(EShTexSampTransUpgradeTextureRemoveSampler);
    }

    shader.setShiftImageBinding(compile_option.image_binding_base[stage]);
    shader.setShiftSamplerBinding(compile_option.sampler_binding_base[stage]);
    shader.setShiftTextureBinding(compile_option.texture_binding_base[stage]);
    shader.setShiftUboBinding(compile_option.ubo_binding_base[stage]);
    shader.setShiftSsboBinding(compile_option.ssbo_binding_base[stage]);
    shader.setShiftUavBinding(compile_option.uav_binding_base[stage]);

    shader.setEnvClient(compile_option.client, compile_option.client_version);
    shader.setEnvTarget(glslang::EShTargetSpv, compile_option.target_spv);
    glslang::EShSource language = compile_option.language;

    // This option will only be used if the Vulkan client is used.
    // If new versions of GL_KHR_vulkan_glsl come out, it would make sense to
    // let callers specify which version to use. For now, just use 100.
    shader.setEnvInput(language, stage, compile_option.client, compile_option.client_version);
    shader.setEnvInputVulkanRulesRelaxed();
    shader.setInvertY(compile_option.invert_y);
    shader.setNanMinMaxClamp(false);

    BuiltinSymbolTable builtin_symbol_table;
    shader.setBuiltinSymbolTable(&builtin_symbol_table);

    DirStackFileIncluder includer;
    for (auto& d : option.include_dirs) {
        includer.pushExternalLocalDirectory(d);
    }

    const EShMessages rules = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgSpvRules | EShMsgVulkanRules);

    auto default_version_ = compile_option.version;
    auto default_profile_ = compile_option.profile;
    auto force_version_profile_ = false;

    bool success = false;

    success = shader.parse(&kDefaultTBuiltInResource, default_version_, default_profile_, force_version_profile_, false,
                           rules, includer);
    if (!success) {
        resource_->info_log = shader.getInfoLog();
        delete resource;
        return false;
    }

    std::cerr << shader.getInfoDebugLog() << std::endl;

    auto* interm = shader.getIntermediate();

#if 0
    {
        TInfoSink sink;
        interm->output(sink, true);
        std::cerr << sink.info.c_str() << std::endl;
        std::cerr << sink.debug.c_str() << std::endl;
    }
#endif

    DocInfoExtractor visitor;
    interm->getTreeRoot()->traverse(&visitor);

    for (auto& s : visitor.globals) {
        auto loc = s->getLoc();
        // fprintf(stderr, "global symbol %s define at %s:%d:%d\n", s->getName().c_str(), loc.getFilename(), loc.line,
        //         loc.column);
        resource->globals.push_back(s);
    }

    for (auto& t : visitor.userdef_types) {
        auto loc = t->getLoc();
        auto const& type = t->getType();
        fprintf(stderr, "user def type %s: %s define at %s:%d:%d\n", type.getTypeName().c_str(),
                type.getCompleteString(true, false, false).c_str(), loc.getFilename(), loc.line, loc.column);
    }

    std::cerr << "DocInfoExtractor found " << visitor.funcs.size() << " function def" << std::endl;
    resource->globals.swap(visitor.globals);
    resource->func_defs.swap(visitor.funcs);
    resource->nodes_by_line.swap(visitor.nodes_by_line);
    resource->userdef_types.swap(visitor.userdef_types);
    builtin_symbol_table.get_all_symbols(resource->builtins);

    resource->uri = resource_->uri;
    resource->version = resource_->version;
    resource->text_ = std::move(resource_->text_);
    resource->lines_ = std::move(resource_->lines_);
    resource->language = resource_->language;

    release_();
    resource_ = resource;

    return true;
}

void Doc::tokenize_()
{
    // tokenize
    auto default_version_ = 110;
    auto default_profile_ = ENoProfile;
    auto default_spv = glslang::SpvVersion();
    auto parser = create_parser(default_version_, default_profile_, language(), default_spv, "main");

    size_t len = resource_->text_.size();
    const char* shader_source = resource_->text_.data();
    glslang::TInputScanner scanner(1, &shader_source, &len);

    parser->ppcontext->setInput(scanner, false);
    parser->parse_context->setScanner(&scanner);

    while (true) {
        YYSTYPE stype;
        int tok = yylex(&stype, *parser->parse_context);
        if (!tok) {
            break;
        }

        auto line = stype.lex.loc.line;
        resource_->tokens_by_line[line].push_back({tok, stype.lex});
    }
}

static Doc::LookupResult lookup_binop(glslang::TIntermBinary* binary, const int line, const int col)
{
    if (binary->getOp() != glslang::EOpIndexDirectStruct) {
        return {Doc::LookupResult::Kind::ERROR};
    }

    auto loc = binary->getLoc();
    if (loc.line != line) {
        return {Doc::LookupResult::Kind::ERROR};
    }

    auto* left = binary->getLeft();
    auto* right = binary->getRight();
    const auto rloc = right->getLoc();
    const auto lloc = left->getLoc();

    bool isref = left->getType().isReference();
    const auto* members = isref ? left->getType().getReferentType()->getStruct() : left->getType().getStruct();
    const auto index = right->getAsConstantUnion()->getConstArray()[0].getIConst();
    const auto field = (*members)[index];

    if (rloc.column <= col && col <= rloc.column + field.type->getFieldName().size()) {
        return {Doc::LookupResult::Kind::FIELD, nullptr, field, nullptr};
    }

    if (auto* left_sym = left->getAsSymbolNode()) {
        if (lloc.line == line && lloc.column <= col && col <= lloc.column + left_sym->getName().size()) {
            return {Doc::LookupResult::Kind::SYMBOL, left_sym, {}, nullptr};
        }
    }

    return {Doc::LookupResult::Kind::ERROR};
}

std::vector<Doc::LookupResult> Doc::lookup_nodes_at(const int line, const int col)
{
    if (!resource_)
        return {};
    std::vector<LookupResult> result;
    if (resource_->nodes_by_line.count(line) <= 0) {
        return result;
    }

    auto& nodes = resource_->nodes_by_line[line];
    for (auto* node : nodes) {
        if (auto sym = node->getAsSymbolNode()) {
            auto loc = sym->getLoc();
            auto endcol = loc.column + sym->getName().length();
            if (line == loc.line && loc.column <= col && col <= endcol) {
                result.push_back({LookupResult::Kind::SYMBOL, sym, {}, nullptr});
            }
        } else if (auto binop = node->getAsBinaryNode()) {
            auto bin_result = lookup_binop(binop, line, col);
            if (bin_result.kind != LookupResult::Kind::ERROR) {
                result.push_back(bin_result);
            }
        } else if (auto unary = node->getAsUnaryNode()) {
            if (unary->getOp() != glslang::EOpDeclare) {
                continue;
            }

            auto const& ty = unary->getType();
            if (!ty.isStruct()) {
                continue;
            }

            auto loc = unary->getLoc();
            auto startcol = loc.column;
            auto endcol = startcol + ty.getTypeName().size();

            if (line == loc.line && startcol <= col && col <= endcol) {
                result.push_back({LookupResult::Kind::TYPE, nullptr, {}, &ty});
            }
        }
    }

    return result;
}

glslang::TSourceLoc Doc::locate_symbol_def(Doc::FunctionDefDesc* func, glslang::TIntermSymbol* target)
{
    if (func) {
        for (auto sym : func->local_defs) {
            if (sym->getId() == target->getId()) {
                return sym->getLoc();
            }
        }
    }

    for (auto global : resource_->globals) {
        if (target->getId() == global->getId()) {
            return global->getLoc();
        }
    }

    return {nullptr, 0, 0};
}

glslang::TSourceLoc Doc::locate_userdef_type(int line, const glslang::TType* ty)
{
    if (!resource_)
        return {};

    auto* func = lookup_func_by_line(line);
    if (func) {
        for (auto* def : func->userdef_types) {
            if (def->getType().getTypeName() == ty->getTypeName()) {
                return def->getLoc();
            }
        }
    }

    for (auto* def : resource_->userdef_types) {
        if (def->getType().getTypeName() == ty->getTypeName()) {
            return def->getLoc();
        }
    }

    return {nullptr, 0, 0};
}

std::vector<glslang::TIntermSymbol*> Doc::lookup_symbols_by_prefix(Doc::FunctionDefDesc* func,
                                                                   std::string const& prefix)
{
    if (!resource_)
        return {};
    std::vector<glslang::TIntermSymbol*> symbols;
    auto match_fn = [&prefix, &symbols](auto vars) {
        for (auto& sym : vars) {
            auto name = sym->getName();
            if (name.size() < prefix.size()) {
                continue;
            }

            std::string sub = name.substr(0, prefix.size()).c_str();
            if (sub == prefix) {
                symbols.push_back(sym);
            }
        }
    };

    match_fn(resource_->globals);
    if (func) {
        match_fn(func->args);
        match_fn(func->local_defs);
    }

    return symbols;
}

std::vector<glslang::TSymbol*> Doc::lookup_builtin_symbols_by_prefix(std::string const& prefix, bool fullname)
{
    if (!resource_)
        return {};
    auto match_fn = [&prefix, fullname](std::string const& name) {
        if (prefix.empty())
            return true;

        return fullname ? prefix == name : prefix == name.substr(0, prefix.size());
    };

    std::vector<glslang::TSymbol*> results;
    for (auto* sym : resource_->builtins) {
        if (match_fn(sym->getName().c_str())) {
            results.push_back(sym);
        }
    }

    return results;
}

glslang::TIntermSymbol* Doc::lookup_symbol_by_name(Doc::FunctionDefDesc* func, std::string const& name)
{
    if (func) {
        for (auto* def : func->local_defs) {
            if (name == def->getName().c_str()) {
                return def;
            }
        }

        for (auto* def : func->args) {
            if (name == def->getName().c_str()) {
                return def;
            }
        }
    }

    for (auto& sym : resource_->globals) {
        if (name == sym->getName().c_str()) {
            return sym;
        }
    }

    return nullptr;
}

Doc::FunctionDefDesc* Doc::lookup_func_by_line(int line)
{
    if (!resource_)
        return nullptr;
    for (auto& func : resource_->func_defs) {
        if (func.start.line <= line && func.end.line >= line) {
            return &func;
        }
    }
    return nullptr;
}
