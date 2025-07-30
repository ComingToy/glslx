#ifndef __GLSLX_WORKSPACE_HPP__
#define __GLSLX_WORKSPACE_HPP__
#include "args.hpp"
#include "doc.hpp"
#include <map>
#include <vector>

class Workspace {
    struct CompileCommand {
        std::string directory;
        std::string command;
        std::string file;
        std::string output;
    };

    std::string root_;
    std::map<std::string, Doc> docs_;
    std::map<std::string, CompileOption> compile_options_;
    void parse_compile_options(std::vector<CompileCommand> const& compile_commands);

public:
    Workspace();
    Workspace(const Workspace&) = delete;
    Workspace(Workspace&&) = delete;
    Workspace& operator=(const Workspace&) = delete;
    Workspace& operator=(Workspace&&) = delete;

    bool init(std::string const& root);

    void update_doc(std::string const& uri, const int version, std::string const& text);
    void add_doc(Doc&& doc);
    std::tuple<bool, Doc*> save_doc(std::string const& uri, const int version);
    Doc* get_doc(std::string const& uri);
    std::string const& get_root() const;
    void set_root(std::string const& root);
    glslang::TSourceLoc locate_symbol_def(std::string const& uri, const int line, const int col);
    std::vector<Doc::LookupResult> lookup_nodes_at(std::string const& uri, const int line, const int col);

    Doc::FunctionDefDesc* get_func_by_line(std::string const& uri, const int line);
    glslang::TIntermSymbol* lookup_symbol_by_name(std::string const& uri, Doc::FunctionDefDesc* func,
                                                  std::string const& name);
    std::vector<glslang::TIntermSymbol*> lookup_symbols_by_prefix(std::string const& uri, Doc::FunctionDefDesc* func,
                                                                  std::string const& prefix);
    std::string get_sentence(std::string const& uri, const int line, const int col, int breakc = ';');
    const CompileOption& get_compile_option(std::string const& uri);
};
#endif
