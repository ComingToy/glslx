#ifndef __GLSLX_DOC_HPP__
#define __GLSLX_DOC_HPP__
#include "args.hpp"
#include "compute_inactive.hpp"
#include "extractors.hpp"
#include "glslang/MachineIndependent/localintermediate.h"
#include "glslang/Public/ShaderLang.h"
#include "parser.hpp"
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

class Doc {
public:
    using FunctionDefDesc = DocInfoExtractor::FunctionDefDesc;
    Doc();
    Doc(std::string const& uri, const int version, std::string const& text);
    Doc(const Doc& rhs);
    Doc(Doc&& rhs);
    Doc& operator=(const Doc& doc);
    Doc& operator=(Doc&& doc);
    virtual ~Doc();

    bool parse(CompileOption const& compile_option);
    void update(const int version, std::string const& text)
    {
        if (resource_->version >= version)
            return;
        resource_->version = version;
        set_text(text);
        // tokenize_();
    }

    int version() const { return resource_->version; }
    std::vector<std::string> const& lines() const { return resource_->lines_; }
    auto const& inactive_blocks() const { return resource_->inactive_blocks_; }
    const char* text()
    {
        if (resource_)
            return resource_->text_.c_str();
        return nullptr;
    }
    std::string const& uri() const { return resource_->uri; }
    EShLanguage language() const { return resource_->language; }
    void set_version(const int version) { resource_->version = version; }
    void set_text(std::string const& text);
    void set_uri(std::string const& uri) { resource_->uri = uri; }

    std::vector<glslang::TIntermSymbol*> lookup_symbols_by_prefix(Doc::FunctionDefDesc* func,
                                                                  std::string const& prefix);
    std::vector<glslang::TSymbol*> lookup_builtin_symbols_by_prefix(std::string const& prefix, bool fullname = false);
    FunctionDefDesc* lookup_func_by_line(int line);
    const std::vector<FunctionDefDesc>& func_defs() { return resource_->func_defs; }
    const std::vector<glslang::TIntermSymbol*>& userdef_types() { return resource_->userdef_types; }
    const std::vector<glslang::TIntermSymbol*>& globals() { return resource_->globals; }

    glslang::TIntermSymbol* lookup_symbol_by_name(Doc::FunctionDefDesc* func, std::string const& name);
    glslang::TIntermediate* intermediate()
    {
        return (resource_ && resource_->shader) ? resource_->shader->getIntermediate() : nullptr;
    }

    const char* info_log() { return resource_ ? resource_->info_log.c_str() : ""; }

    struct LookupResult {
        enum class Kind { SYMBOL, FIELD, TYPE, ERROR } kind;
        glslang::TIntermSymbol* sym;
        glslang::TTypeLoc field;
        const glslang::TType* ty;
    };

    std::vector<LookupResult> lookup_nodes_at(const int line, const int col);
    glslang::TSourceLoc locate_symbol_def(Doc::FunctionDefDesc* func, glslang::TIntermSymbol* use);
    glslang::TSourceLoc locate_userdef_type(int line, const glslang::TType* use);

    using Range = ComputeInactiveHelper::Range;

private:
    typedef decltype(YYSTYPE::lex) lex_info_type;
    struct Token {
        int tok;
        lex_info_type lex;
    };
    struct __Resource {
        std::string uri;
        int version;
        std::string text_;
        std::vector<std::string> lines_;
        EShLanguage language;

        std::unique_ptr<glslang::TShader> shader;

        std::map<int, std::vector<TIntermNode*>> nodes_by_line;
        std::vector<FunctionDefDesc> func_defs;
        std::vector<glslang::TIntermSymbol*> globals;
        std::vector<glslang::TIntermSymbol*> userdef_types;
        std::map<int, std::vector<Token>> tokens_by_line;
        std::vector<glslang::TSymbol*> builtins;
        std::string info_log;
        std::vector<Range> inactive_blocks_;
        int ref = 1;
    };

    __Resource* resource_;
    void infer_language_();
    void tokenize_(CompileOption const& option);
    void release_();

    LookupResult lookup_node_in_struct(const int line, const int col);
    void compute_inactive_blocks_(std::map<std::string, std::map<int, int>>& cond_res);
};
#endif
