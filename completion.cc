#include "completion.hpp"
#include "glslang/Include/Common.h"
#include "parser.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include <memory>
#include <stack>
#include <tuple>
#include <vector>

std::vector<const char*> __keywords = {
    "const", "uniform", "tileImageEXT", "buffer", "in", "out", "smooth", "flat", "centroid", "invariant", "packed",
    "resource", "inout", "struct", "break", "continue", "do", "for", "while", "switch", "case", "default", "if", "else",
    "discard", "terminateInvocation", "terminateRayEXT", "ignoreIntersectionEXT", "return", "void", "bool", "float",
    "int", "bvec2", "bvec3", "bvec4", "vec2", "vec3", "vec4", "ivec2", "ivec3", "ivec4", "mat2", "mat3", "mat4", "true",
    "false", "layout", "shared", "highp", "mediump", "lowp", "superp", "precision", "mat2x2", "mat2x3", "mat2x4",
    "mat3x2", "mat3x3", "mat3x4", "mat4x2", "mat4x3", "mat4x4", "uint", "uvec2", "uvec3", "uvec4",

    "nonuniformEXT", "demote", "attribute", "varying", "noperspective", "coherent", "devicecoherent",
    "queuefamilycoherent", "workgroupcoherent", "subgroupcoherent", "shadercallcoherent", "nonprivate", "restrict",
    "readonly", "writeonly", "atomic_uint", "volatile", "nontemporal", "patch", "sample", "subroutine", "dmat2",
    "dmat3", "dmat4", "dmat2x2", "dmat2x3", "dmat2x4", "dmat3x2", "dmat3x3", "dmat3x4", "dmat4x2", "dmat4x3", "dmat4x4",
    "image1D", "iimage1D", "uimage1D", "image2D", "iimage2D", "uimage2D", "image3D", "iimage3D", "uimage3D",
    "image2DRect", "iimage2DRect", "uimage2DRect", "imageCube", "iimageCube", "uimageCube", "imageBuffer",
    "iimageBuffer", "uimageBuffer", "image1DArray", "iimage1DArray", "uimage1DArray", "image2DArray", "iimage2DArray",
    "uimage2DArray", "imageCubeArray", "iimageCubeArray", "uimageCubeArray", "image2DMS", "iimage2DMS", "uimage2DMS",
    "image2DMSArray", "iimage2DMSArray", "uimage2DMSArray", "i64image1D", "u64image1D", "i64image2D", "u64image2D",
    "i64image3D", "u64image3D", "i64image2DRect", "u64image2DRect", "i64imageCube", "u64imageCube", "i64imageBuffer",
    "u64imageBuffer", "i64image1DArray", "u64image1DArray", "i64image2DArray", "u64image2DArray", "i64imageCubeArray",
    "u64imageCubeArray", "i64image2DMS", "u64image2DMS", "i64image2DMSArray", "u64image2DMSArray", "double", "dvec2",
    "dvec3", "dvec4", "int64_t", "uint64_t", "i64vec2", "i64vec3", "i64vec4", "u64vec2", "u64vec3", "u64vec4",

    // GL_EXT_shader_explicit_arithmetic_types
    "int8_t", "i8vec2", "i8vec3", "i8vec4", "uint8_t", "u8vec2", "u8vec3", "u8vec4",

    "int16_t", "i16vec2", "i16vec3", "i16vec4", "uint16_t", "u16vec2", "u16vec3", "u16vec4",

    "int32_t", "i32vec2", "i32vec3", "i32vec4", "uint32_t", "u32vec2", "u32vec3", "u32vec4",

    "float16_t", "f16vec2", "f16vec3", "f16vec4", "f16mat2", "f16mat3", "f16mat4", "f16mat2x2", "f16mat2x3",
    "f16mat2x4", "f16mat3x2", "f16mat3x3", "f16mat3x4", "f16mat4x2", "f16mat4x3", "f16mat4x4",

    "bfloat16_t", "bf16vec2", "bf16vec3", "bf16vec4",

    "floate5m2_t", "fe5m2vec2", "fe5m2vec3", "fe5m2vec4",

    "floate4m3_t", "fe4m3vec2", "fe4m3vec3", "fe4m3vec4",

    "float32_t", "f32vec2", "f32vec3", "f32vec4", "f32mat2", "f32mat3", "f32mat4", "f32mat2x2", "f32mat2x3",
    "f32mat2x4", "f32mat3x2", "f32mat3x3", "f32mat3x4", "f32mat4x2", "f32mat4x3", "f32mat4x4", "float64_t", "f64vec2",
    "f64vec3", "f64vec4", "f64mat2", "f64mat3", "f64mat4", "f64mat2x2", "f64mat2x3", "f64mat2x4", "f64mat3x2",
    "f64mat3x3", "f64mat3x4", "f64mat4x2", "f64mat4x3", "f64mat4x4",

    // GL_EXT_spirv_intrinsics
    "spirv_instruction", "spirv_execution_mode", "spirv_execution_mode_id", "spirv_decorate", "spirv_decorate_id",
    "spirv_decorate_string", "spirv_type", "spirv_storage_class", "spirv_by_reference", "spirv_literal",

    "sampler2D", "samplerCube", "samplerCubeShadow", "sampler2DArray", "sampler2DArrayShadow", "isampler2D",
    "isampler3D", "isamplerCube", "isampler2DArray", "usampler2D", "usampler3D", "usamplerCube", "usampler2DArray",
    "sampler3D", "sampler2DShadow",

    "texture2D", "textureCube", "texture2DArray", "itexture2D", "itexture3D", "itextureCube", "itexture2DArray",
    "utexture2D", "utexture3D", "utextureCube", "utexture2DArray", "texture3D",

    "sampler", "samplerShadow",

    "textureCubeArray", "itextureCubeArray", "utextureCubeArray", "samplerCubeArray", "samplerCubeArrayShadow",
    "isamplerCubeArray", "usamplerCubeArray", "sampler1DArrayShadow", "isampler1DArray", "usampler1D", "isampler1D",
    "usampler1DArray", "samplerBuffer", "isampler2DRect", "usampler2DRect", "isamplerBuffer", "usamplerBuffer",
    "sampler2DMS", "isampler2DMS", "usampler2DMS", "sampler2DMSArray", "isampler2DMSArray", "usampler2DMSArray",
    "sampler1D", "sampler1DShadow", "sampler2DRect", "sampler2DRectShadow", "sampler1DArray",

    "samplerExternalOES",        // GL_OES_EGL_image_external
    "__samplerExternal2DY2YEXT", // GL_EXT_YUV_target

    "itexture1DArray", "utexture1D", "itexture1D", "utexture1DArray", "textureBuffer", "itexture2DRect",
    "utexture2DRect", "itextureBuffer", "utextureBuffer", "texture2DMS", "itexture2DMS", "utexture2DMS",
    "texture2DMSArray", "itexture2DMSArray", "utexture2DMSArray", "texture1D", "texture2DRect", "texture1DArray",

    "attachmentEXT", "iattachmentEXT", "uattachmentEXT",

    "subpassInput", "subpassInputMS", "isubpassInput", "isubpassInputMS", "usubpassInput", "usubpassInputMS",

    "f16sampler1D", "f16sampler2D", "f16sampler3D", "f16sampler2DRect", "f16samplerCube", "f16sampler1DArray",
    "f16sampler2DArray", "f16samplerCubeArray", "f16samplerBuffer", "f16sampler2DMS", "f16sampler2DMSArray",
    "f16sampler1DShadow", "f16sampler2DShadow", "f16sampler2DRectShadow", "f16samplerCubeShadow",
    "f16sampler1DArrayShadow", "f16sampler2DArrayShadow", "f16samplerCubeArrayShadow",

    "f16image1D", "f16image2D", "f16image3D", "f16image2DRect", "f16imageCube", "f16image1DArray", "f16image2DArray",
    "f16imageCubeArray", "f16imageBuffer", "f16image2DMS", "f16image2DMSArray",

    "f16texture1D", "f16texture2D", "f16texture3D", "f16texture2DRect", "f16textureCube", "f16texture1DArray",
    "f16texture2DArray", "f16textureCubeArray", "f16textureBuffer", "f16texture2DMS", "f16texture2DMSArray",

    "f16subpassInput", "f16subpassInputMS", "__explicitInterpAMD", "pervertexNV", "pervertexEXT", "precise",

    "rayPayloadNV", "rayPayloadEXT", "rayPayloadInNV", "rayPayloadInEXT", "hitAttributeNV", "hitAttributeEXT",
    "callableDataNV", "callableDataEXT", "callableDataInNV", "callableDataInEXT", "accelerationStructureNV",
    "accelerationStructureEXT", "rayQueryEXT", "perprimitiveNV", "perviewNV", "taskNV", "perprimitiveEXT",
    "taskPayloadSharedEXT",

    "fcoopmatNV", "icoopmatNV", "ucoopmatNV",

    "coopmat",

    "hitObjectNV", "hitObjectAttributeNV",

    "__function", "tensorLayoutNV", "tensorViewNV",

    "coopvecNV", "require", "binding", "include", "extension", "local_size_x_id", "local_size_y_id", "local_size_z_id",
    "constant_id", "push_constant"};

extern int yylex(YYSTYPE*, glslang::TParseContext&);
struct InputStackState {
    int kind; // 0 for lex. 1 for struct 2 for arr 3 scalar
    const glslang::TType* ttype;
    const YYSTYPE* stype;
    int tok;
    int reduce_n_ = 0;
};

class CompletionHelper {
public:
    CompletionHelper(Doc& doc, const int line, const int col, std::vector<const char*> const& keywrods,
                     std::vector<const char*> const& extentions)
        : doc_(doc), line_(line), col_(col), keywrods_(keywrods), extentions_(extentions)
    {
    }

    void do_complete(std::vector<std::tuple<YYSTYPE, int>> const& lex_info, CompletionResultSet& results)
    {
        //very tiny varaible exp parser
        std::stack<InputStackState> input_stack;

#define START 0
#define END -1
#define EXPECT_DOT_LBRACKET 1
#define EXPECT_IDENTIFIER 2
#define EXPECT_RBRACKET 3

        int state = START;
        //0 tok for start, -1 tok for end
        for (int i = 0; i < lex_info.size(); ++i) {
            auto const& [stype, tok] = lex_info[i];
            if (tok == -1) {
                //do complete at end
                if (input_stack.top().kind == 0) {
                    do_complete_exp_(input_stack, results);
                }

                return;
            }

            switch (state) {
            case START:
                if (tok == IDENTIFIER) {
                    input_stack.push({0, nullptr, &stype, tok});
                    state = EXPECT_DOT_LBRACKET;
                } else {
                    // err
                    return;
                }
                break;
            case EXPECT_DOT_LBRACKET:
                if (tok == DOT) {
                    if (!reduce_struct_(input_stack)) {
                        return;
                    }

                    input_stack.push({0, nullptr, &stype, tok});
                    state = EXPECT_IDENTIFIER;
                    break;
                } else if (tok == LEFT_BRACKET) {
                    if (!reduce_arr_(input_stack)) {
                        return;
                    }
                    input_stack.push({0, nullptr, &stype, tok});
                    state = EXPECT_RBRACKET;
                } else {
                    return;
                }
                break;
            case EXPECT_IDENTIFIER: {
                auto const& [nstype, ntok] = lex_info[i + 1];
                if (ntok == -1) {
                    input_stack.push({0, nullptr, &stype, tok});
                } else if (tok == IDENTIFIER) {
                    if (input_stack.top().kind == 0 && input_stack.top().tok == DOT) {
                        input_stack.push({0, nullptr, &stype, tok});
                        if (!reduce_field_(input_stack)) {
                            return;
                        }
                        state = EXPECT_DOT_LBRACKET;
                    }
                } else {
                    return;
                }
            } break;
            case EXPECT_RBRACKET:
                if (tok == RIGHT_BRACKET) {
                    if (!reduce_subscript_(input_stack)) {
                        return;
                    }
                    state = EXPECT_DOT_LBRACKET;
                }
                break;
            default:
                break;
            }
        }
    }

private:
    Doc& doc_;
    const int line_, col_;
    std::vector<const char*> const& keywrods_;
    std::vector<const char*> const& extentions_;

    void do_complete_exp_(std::stack<InputStackState>& input_stack, CompletionResultSet& results)
    {
        if (input_stack.top().kind != 0) {
            return;
        }

        auto input = input_stack.top();
        input_stack.pop();

        if (input.tok == IDENTIFIER) {
            std::string prefix = input.stype->lex.string->c_str();
            if (input_stack.empty()) {
                do_complete_var_prefix_(prefix, results);
                do_complete_type_prefix_(prefix, results);
                do_complete_keywords_prefix_(prefix, results);
                do_complete_builtin_prefix_(prefix, results);
                do_complete_extention_prefix_(prefix, results);
                return;
            }

            auto top = input_stack.top();
            input_stack.pop();
            if (top.kind != 0) {
                auto* func = doc_.lookup_func_by_line(line_);
                auto symbols = doc_.lookup_symbols_by_prefix(func, input.stype->lex.string->c_str());
                for (auto* sym : symbols) {
                    std::string label = sym->getName().c_str();
                    std::string detail = sym->getType().getCompleteString(true, false, false).c_str();
                    CompletionResult r = {label, CompletionItemKind::Variable, detail, "", label};
                    results.variables.emplace_back(r);
                }
            } else if (top.tok == DOT) {
                if (input_stack.empty())
                    return;
                auto& [kind, ttype, stype, tok, _] = input_stack.top();
                if (kind != 1) {
                    return;
                }

                do_complete_struct_field_(ttype, prefix, results);
            }
        } else if (input.tok == DOT) {
            if (input_stack.empty())
                return;

            auto& [kind, ttype, stype, tok, _] = input_stack.top();
            if (kind != 1) {
                return;
            }

            if (ttype->isVector()) {
                std::string tyname = ttype->getBasicTypeString().c_str();
                const char* fields[] = {"x", "y", "z", "w"};
                for (auto i = 0; i < ttype->getVectorSize(); ++i) {
                    results.variables.push_back({fields[i], CompletionItemKind::Field, tyname + " " + fields[i], "",
                                                 fields[i], InsertTextFormat::PlainText});
                }
            } else {
                auto& members = *ttype->getStruct();
                for (int i = 0; i < members.size(); ++i) {
                    auto field = members[i].type;
                    auto label = field->getFieldName().c_str();
                    auto kind = CompletionItemKind::Field;
                    auto detail = field->getCompleteString(true, false, false);
                    auto doc = "";
                    results.variables.push_back({label, kind, detail.c_str(), doc, label, InsertTextFormat::PlainText});
                }
            }
        }
    }

    void do_complete_var_prefix_(std::string const& prefix, CompletionResultSet& results)
    {
        auto* func = doc_.lookup_func_by_line(line_);
        auto symbols = doc_.lookup_symbols_by_prefix(func, prefix);
        for (auto const& sym : symbols) {
            auto detail = sym->getType().getCompleteString(true, false, false);
            auto label = sym->getName().c_str();

            CompletionResult r = {label, CompletionItemKind::Variable, detail.c_str(), "", label};
            results.variables.push_back(r);
        }

        auto match_prefix = [&prefix](std::string const& field) {
            if (prefix.empty())
                return true;

            return prefix == field.substr(0, prefix.size());
        };

        auto& func_defs = doc_.func_defs();
        auto norm_func_name = [](std::string const& fname) {
            auto pos = fname.find("(");
            if (pos == std::string::npos) {
                return fname;
            }

            return std::string(fname.begin(), fname.begin() + pos);
        };

        for (const auto& func : func_defs) {
            if (!match_prefix(func.def->getName().c_str())) {
                continue;
            }

            auto label = norm_func_name(func.def->getName().c_str());
            std::string return_type;
            auto const& rtype = func.def->getType();
            if (rtype.isStruct()) {
                return_type = rtype.getTypeName();
            } else {
                return_type = rtype.getBasicTypeString();
            }

            std::string args_list_snippet;
            std::string args_list;
            for (int i = 0; i < func.args.size(); ++i) {
                auto* arg = func.args[i];
                char buf[128];
                auto const& arg_type = arg->getType();
                const char* arg_type_str;
                if (arg_type.isStruct()) {
                    arg_type_str = arg_type.getTypeName().c_str();
                } else {
                    arg_type_str = arg_type.getBasicTypeString().c_str();
                }

                snprintf(buf, sizeof(buf), "${%d:%s %s}", i + 1, arg_type_str, arg->getName().c_str());

                args_list_snippet += buf;
                args_list_snippet += ", ";
                args_list = args_list + arg_type_str + " " + arg->getName().c_str() + ", ";
            }

            if (args_list_snippet.size() > 2) {
                args_list_snippet.pop_back();
                args_list_snippet.pop_back();
                args_list.pop_back();
                args_list.pop_back();
            }
            std::string detail = return_type + " " + label + "(" + args_list + ")";
            std::string insert_text = label + "(" + args_list_snippet + ")";

            CompletionResult r = {label,       CompletionItemKind::Function, detail, "",
                                  insert_text, InsertTextFormat::Snippet};
            results.funcs.push_back(r);
        }
    }

    bool reduce_field_(std::stack<InputStackState>& input_stack)
    {
        auto field = input_stack.top();
        input_stack.pop();
        auto dot = input_stack.top();
        input_stack.pop();
        auto s = input_stack.top();
        input_stack.pop();

        auto& members = *s.ttype->getStruct();
        int i = 0;
        int kind = 0;

        if (field.kind != 0 || field.tok != IDENTIFIER) {
            return false;
        }

        if (dot.kind != 0 || dot.tok != DOT) {
            return false;
        }

        if (s.kind != 1 || !s.ttype->isStruct()) {
            return false;
        }

        glslang::TType* fty = nullptr;
        for (i = 0; i < members.size(); ++i) {
            auto* p = members[i].type;
            if (p->isReference()) {
                p = p->getReferentType();
            }

            if (p->getFieldName() == field.stype->lex.string->c_str()) {
                fty = p;
                break;
            }
        }

        if (!fty) {
            return false;
        }

        if (fty->isArray()) {
            kind = 2;
        } else if (fty->isStruct()) {
            kind = 1;
        } else if (fty->isScalar()) {
            kind = 3;
        }

        input_stack.push({kind, fty, nullptr, 0});
        return true;
    }

    bool reduce_subscript_(std::stack<InputStackState>& input_stack)
    {
        while (!input_stack.empty() && input_stack.top().tok != LEFT_BRACKET) {
            input_stack.pop();
        }

        if (input_stack.empty()) {
            return false;
        }

        input_stack.pop();
        auto& top = input_stack.top();
        if (top.kind != 2) {
            return false;
        }

        auto* ttype = top.ttype;
        auto sizes = ttype->getArraySizes();
        if (top.reduce_n_ >= sizes->getNumDims()) {
            return false;
        }

        top.reduce_n_ += 1;

        if (top.reduce_n_ == sizes->getNumDims()) {
            if (top.ttype->isStruct()) {
                top.kind = 1;
            } else {
                top.kind = 3;
            }
        }

#if 0
    if (node->isArray()) {
        top.kind = 2;
    } else if (node->isStruct()) {
        top.kind = 1;
    } else {
        top.kind = 3;
    }

    top.ttype = &node->getType();
#endif
        return true;
    }

    bool reduce_arr_(std::stack<InputStackState>& input_stack)
    {
        auto& top = input_stack.top();
        if (top.kind == 2) {
            return true;
        }

        if (top.kind != 0 || top.tok != IDENTIFIER) {
            return false;
        }

        auto* func = doc_.lookup_func_by_line(line_);
        auto* sym = doc_.lookup_symbol_by_name(func, top.stype->lex.string->c_str());
        if (!sym) {
            return false;
        }

        const auto* type = &sym->getType();
        if (type->isReference()) {
            type = type->getReferentType();
        }

        if (!type->isArray()) {
            return false;
        }

        top.kind = 2;
        top.ttype = type;
        return true;
    }

    bool reduce_struct_(std::stack<InputStackState>& input_stack)
    {
        auto& top = input_stack.top();

        // not lex tok
        if (top.kind != 0 && top.kind != 1) {
            return false;
        }

        if (top.kind == 1)
            return true;

        auto tok = top.tok;
        if (tok != IDENTIFIER) {
            return false;
        }

        const glslang::TType* type = nullptr;
        auto* func = doc_.lookup_func_by_line(line_);
        auto* sym = doc_.lookup_symbol_by_name(func, top.stype->lex.string->c_str());
        auto* global = doc_.lookup_symbol_by_name(nullptr, top.stype->lex.string->c_str());
        auto builtin = doc_.lookup_builtin_symbols_by_prefix(top.stype->lex.string->c_str(), true);

        if (sym) {
            type = &sym->getType();
        } else if (global) {
            type = &global->getType();
        } else if (!builtin.empty()) {
            auto* sym = builtin.front();
            if (auto* var = sym->getAsVariable()) {
                type = &var->getType();
            }
        }

        if (!type)
            return false;

        if (type->isReference()) {
            type = type->getReferentType();
        }

        if (!type->isStruct() && !type->isVector()) {
            return false;
        }

        top.kind = 1;
        top.ttype = type;
        return true;
    }

    static bool match_prefix(std::string const& s, std::string const& prefix)
    {
        if (prefix.empty())
            return true;

        return prefix == s.substr(0, prefix.size());
    }

    void do_complete_keywords_prefix_(std::string const& prefix, CompletionResultSet& result)
    {
        for (auto& keyword : keywrods_) {
            if (match_prefix(keyword, prefix)) {
                result.keywords.push_back(
                    {keyword, CompletionItemKind::Keyword, "", "", keyword, InsertTextFormat::PlainText});
            }
        }
    }

    void do_complete_type_prefix_(std::string const& prefix, CompletionResultSet& results)
    {
        auto const& userdef_types = doc_.userdef_types();
        for (auto* sym : userdef_types) {
            auto loc = sym->getLoc();
            auto const& ty = sym->getType();
            auto const* tyname = ty.getTypeName().c_str();
            if (!match_prefix(tyname, prefix))
                continue;

            CompletionResult r = {
                tyname, CompletionItemKind::Struct, ty.getCompleteString(true, false, false).c_str(), "",
                tyname, InsertTextFormat::PlainText};
            results.types.push_back(r);
        }
    }

    void do_complete_builtin_prefix_(std::string const& prefix, CompletionResultSet& results)
    {
        auto builtins = doc_.lookup_builtin_symbols_by_prefix(prefix);

        for (auto* sym : builtins) {
            if (auto* var = sym->getAsVariable()) {
                auto* label = var->getName().c_str();
                std::string detail = var->getType().getCompleteString(true).c_str();
                CompletionResult result = {label, CompletionItemKind::Variable, detail, "",
                                           label, InsertTextFormat::PlainText};
                results.builtins.push_back(result);
            } else if (const auto* func = sym->getAsFunction()) {
                std::string func_name = func->getName().c_str();
                std::string return_type;
                if (func->getType().isStruct()) {
                    return_type = func->getType().getTypeName().c_str();
                } else {
                    return_type = func->getType().getBasicTypeString().c_str();
                }

                std::string args_list;
                std::string args_list_snippet;

                for (int i = 0; i < func->getParamCount(); ++i) {
                    const auto& arg = (*func)[i];
                    char buf[128];
                    auto const& arg_type = *arg.type;
                    const char* arg_type_str;
                    if (arg_type.isStruct()) {
                        arg_type_str = arg_type.getTypeName().c_str();
                    } else if (arg_type.isVector()) {
                        arg_type_str = arg_type.getCompleteString(true, false, false).c_str();
                    } else {
                        arg_type_str = arg_type.getBasicTypeString().c_str();
                    }

                    snprintf(buf, sizeof(buf), "${%d:%s %s}", i + 1, arg_type_str, arg.name ? arg.name->c_str() : "");

                    args_list_snippet += buf;
                    args_list_snippet += ", ";
                    args_list = args_list + arg_type_str + " " + (arg.name ? arg.name->c_str() : "") + ", ";
                }

                if (args_list_snippet.size() > 2) {
                    args_list_snippet.pop_back();
                    args_list_snippet.pop_back();
                    args_list.pop_back();
                    args_list.pop_back();
                }
                std::string detail = return_type + " " + func_name + "(" + args_list + ")";
                std::string insert_text = func_name + "(" + args_list_snippet + ")";

                std::cerr << "complete builtin func: " << insert_text << std::endl;

                CompletionResult r = {func_name,   CompletionItemKind::Function, detail, "",
                                      insert_text, InsertTextFormat::Snippet};
                results.builtins.push_back(r);
            }
        }
    }

    void do_complete_struct_field_(const glslang::TType* ttype, std::string const& prefix, CompletionResultSet& results)
    {
        auto match_prefix = [&prefix](std::string const& field) {
            if (prefix.empty())
                return true;

            return prefix == field.substr(0, prefix.size());
        };

        auto& members = *ttype->getStruct();
        for (int i = 0; i < members.size(); ++i) {
            auto field = members[i].type;
            auto label = field->getFieldName().c_str();
            auto kind = CompletionItemKind::Field;
            auto detail = field->getCompleteString(true, false, false);
            auto doc = "";
            if (match_prefix(label)) {
                results.variables.push_back({label, kind, detail.c_str(), doc, label, InsertTextFormat::PlainText});
            }
        }
    }

    void do_complete_extention_prefix_(std::string const& prefix, CompletionResultSet& results)
    {
        for (auto e : extentions_) {
            if (match_prefix(e, prefix)) {
                results.builtins.push_back({e, CompletionItemKind::Text, "", "", e, InsertTextFormat::PlainText});
            }
        }
    }
};

void completion(Doc& doc, std::string const& anon_prefix, std::string const& input, const int line, const int col,
                CompletionResultSet& results)
{
    const char* source = input.data();
    size_t len = input.size();
    glslang::TInputScanner userInput(1, &source, &len);

    int version;
    EProfile profile;
    EShLanguage stage;
    glslang::SpvVersion spvVersion;
    std::string entrypoint = "main";
    bool notFirstToken;

    if (auto* interm = doc.intermediate()) {
        version = interm->getVersion();
        profile = interm->getProfile();
        stage = interm->getStage();
        spvVersion = interm->getSpv();
        entrypoint = interm->getEntryPointName();
    } else {
        const char* text = doc.text();
        if (!text) {
            return;
        }

        size_t len = strlen(text);
        glslang::TInputScanner versionScanner(1, &text, &len);
        versionScanner.scanVersion(version, profile, notFirstToken);
    }

    if (version <= 0) {
        return;
    }

    auto parser_resource = create_parser(version, profile, stage, spvVersion, entrypoint.c_str());
    parser_resource->ppcontext->setInput(userInput, false);
    parser_resource->parse_context->setScanner(&userInput);
    parser_resource->parse_context->initializeExtensionBehavior();
    auto textensions = parser_resource->parse_context->getExtensionList();
    std::vector<const char*> extentions(textensions.cbegin(), textensions.cend());

    std::vector<std::tuple<YYSTYPE, int>> input_toks;
    if (!anon_prefix.empty()) {
        YYSTYPE lex;
        lex.lex.string = glslang::NewPoolTString(anon_prefix.c_str());
        input_toks.push_back({lex, IDENTIFIER});
        input_toks.push_back({YYSTYPE{}, DOT});
    }

    while (true) {
        YYSTYPE stype;
        int tok = yylex(&stype, *parser_resource->parse_context);
        if (!tok) {
            break;
        }

        input_toks.push_back(std::make_tuple(stype, tok));
    }

    if (input_toks.empty())
        return;

    input_toks.push_back({YYSTYPE{}, -1});
    CompletionHelper helper(doc, line, col, __keywords, extentions);
    helper.do_complete(input_toks, results);
}
