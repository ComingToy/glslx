#ifndef __GLSLD_COMPLETION_H__
#define __GLSLD_COMPLETION_H__

#include "doc.hpp"
#include "lsp_defs.hpp"

extern void completion(Doc& doc, std::string const& anon_prefix, std::string const& input, const int line,
                       const int col, CompletionResultSet& results);
#endif
