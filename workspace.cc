#include "workspace.hpp"
#include "glsld/doc.hpp"
#include <iostream>
#include <tuple>
#include <vector>

Workspace::Workspace() : root_("/") {}

void Workspace::set_root(std::string const& root) { root_ = root; }
std::string const& Workspace::get_root() const { return root_; }
void Workspace::update_doc(std::string const& uri, const int version, std::string const& text)
{
    if (docs_.count(uri) > 0) {
        docs_[uri].update(version, text);
    } else {
        add_doc(Doc(uri, version, text));
    }
}

std::tuple<bool, Doc*> Workspace::save_doc(std::string const& uri, const int version)
{
    if (docs_.count(uri) > 0) {
        auto& doc = docs_[uri];
        if (doc.version() == version) {
            bool ret = doc.parse({get_root()});
            return std::make_tuple(ret, &doc);
        }
        return std::make_tuple(true, &doc);
    }

    return std::make_tuple(true, nullptr);
}

void Workspace::add_doc(Doc&& doc) { docs_[doc.uri()] = std::move(doc); }

std::vector<Doc::LookupResult> Workspace::lookup_nodes_at(std::string const& uri, const int line, const int col)
{
    if (docs_.count(uri)) {
        return docs_[uri].lookup_nodes_at(line, col);
    }
    return {};
}

glslang::TSourceLoc Workspace::locate_symbol_def(std::string const& uri, const int line, const int col)
{
    if (docs_.count(uri) <= 0)
        return {.name = nullptr, .line = 0, .column = 0};

    auto nodes = lookup_nodes_at(uri, line, col);
    auto* func = docs_[uri].lookup_func_by_line(line);

    for (auto& node : nodes) {
        if (node.kind == Doc::LookupResult::Kind::SYMBOL) {
            return docs_[uri].locate_symbol_def(func, node.sym);
        } else if (node.kind == Doc::LookupResult::Kind::FIELD) {
            return node.field.loc;
        } else if (node.kind == Doc::LookupResult::Kind::TYPE) {
            return docs_[uri].locate_userdef_type(node.ty);
        }
    }

    return {.name = nullptr, .line = 0, .column = 0};
}

std::string Workspace::get_sentence(std::string const& uri, const int line, const int col, int breakc)
{
    if (docs_.count(uri) <= 0)
        return "";

    const auto& lines = docs_[uri].lines();
    std::string const& text = lines[line];
    std::cerr << "completion at line " << line << ": " << text << std::endl
              << "text size: " << text.size() << ", col: " << col;
    if (text.size() < col) {
        return {};
    }

    auto pos = text.rbegin() + (text.size() - col);
    std::vector<char> buf;
    for (; pos != text.rend(); ++pos) {
        if (*pos == breakc)
            break;
        buf.push_back(*pos);
    }

    std::string prefix(buf.rbegin(), buf.rend());
    return prefix;
}

std::vector<glslang::TIntermSymbol*>
Workspace::lookup_symbols_by_prefix(std::string const& uri, Doc::FunctionDefDesc* func, std::string const& prefix)
{
    auto syms = docs_[uri].lookup_symbols_by_prefix(func, prefix);
    for (auto* sym : syms) {
        std::cerr << "find symbol: " << sym->getName() << std::endl;
    }
    return syms;
}

glslang::TIntermSymbol* Workspace::lookup_symbol_by_name(std::string const& uri, Doc::FunctionDefDesc* func,
                                                         std::string const& name)
{
    if (docs_.count(uri) <= 0) {
        return nullptr;
    }

    auto& doc = docs_[uri];
    return doc.lookup_symbol_by_name(func, name);
}

Doc::FunctionDefDesc* Workspace::get_func_by_line(const std::string& uri, const int line)
{
    if (docs_.count(uri) <= 0)
        return nullptr;
    return docs_[uri].lookup_func_by_line(line);
}

Doc* Workspace::get_doc(std::string const& uri)
{
    if (docs_.count(uri) > 0)
        return &docs_[uri];
    else
        return nullptr;
}
