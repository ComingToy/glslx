#ifndef __GLSLX_PARSER_H__
#define __GLSLX_PARSER_H__
#include "glslang/StandAlone/DirStackFileIncluder.h"
#include "glslang/MachineIndependent/ScanContext.h"
#include "glslang/MachineIndependent/glslang_tab.cpp.h"
#include "glslang/MachineIndependent/preprocessor/PpContext.h"
#include <memory>

struct ParserResouce {
    glslang::TSymbolTable* symbol_table;
    glslang::TIntermediate* intermediate;
    glslang::TParseContext* parse_context;
    glslang::TScanContext* scan_context;
    glslang::TPpContext* ppcontext;
    DirStackFileIncluder* includer;

    ParserResouce(glslang::TSymbolTable* symbol_table, glslang::TIntermediate* intermediate,
                  glslang::TParseContext* parse_context, glslang::TScanContext* scan_context,
                  glslang::TPpContext* ppcontext, DirStackFileIncluder* includer)
    {
        this->symbol_table = symbol_table;
        this->intermediate = intermediate;
        this->parse_context = parse_context;
        this->scan_context = scan_context;
        this->ppcontext = ppcontext;
        this->includer = includer;
    }
};

extern std::unique_ptr<ParserResouce> create_parser(const int version, EProfile profile, EShLanguage stage,
                                                    glslang::SpvVersion spvVersion, const char* entrypoint);
#endif
