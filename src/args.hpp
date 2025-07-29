#ifndef __GLSLX_ARGS_HPP__
#define __GLSLX_ARGS_HPP__
#include "glslang/Public/ShaderLang.h"
#include <map>
#include <string>
#include <vector>

struct Binding {
    std::string name;
    int set;
    int binding;
};

extern TBuiltInResource kDefaultTBuiltInResource;

struct CompileOption {
    std::map<std::string, std::string> macros;
    bool auto_bind_uniforms = false;
    bool auto_map_locations = false;
    bool auto_combinded_image_sampler = false;
    std::string entrypoint = "main";
    bool invert_y = false;
    TBuiltInResource limits = kDefaultTBuiltInResource;
    bool nan_clamp = false;
    bool preserve_binddings = false;
    std::map<EShLanguage, std::vector<Binding>> resource_set_binding;
    std::map<EShLanguage, int> cbuffer_binding_base;
    std::map<EShLanguage, int> image_binding_base;
    std::map<EShLanguage, int> sampler_binding_base;
    std::map<EShLanguage, int> ssbo_binding_base;
    std::map<EShLanguage, int> texture_binding_base;
    std::map<EShLanguage, int> uav_binding_base;
    std::map<EShLanguage, int> ubo_binding_base;
    EShLanguage shader_stage = EShLangCount;
    int version = 450;
    EProfile profile = ECoreProfile;
    glslang::EShTargetClientVersion client_version = glslang::EShTargetVulkan_1_0;
    glslang::EShClient client = glslang::EShClientVulkan;
    glslang::EShTargetLanguageVersion target_spv = glslang::EShTargetSpv_1_0;
    glslang::EShSource language = glslang::EShSourceGlsl;
    std::vector<std::string> include_dirs;
    bool suppress_warining = false;
    bool warnings_as_errors = false;
    std::string filename;
};

extern bool parse_compile_options(const int argc, const char* argv[], CompileOption& compile_options);
extern bool parse_compile_options(std::vector<std::string> const& args, CompileOption& compile_options);
extern EShLanguage map_to_stage(std::string_view const& name);
#endif
