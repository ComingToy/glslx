#include "args.hpp"
#include "glslang/Public/ShaderLang.h"
#include <cctype>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

TBuiltInResource kDefaultTBuiltInResource = {
    /*.maxLights = */ 8,        // From OpenGL 3.0 table 6.46.
    /*.maxClipPlanes = */ 6,    // From OpenGL 3.0 table 6.46.
    /*.maxTextureUnits = */ 2,  // From OpenGL 3.0 table 6.50.
    /*.maxTextureCoords = */ 8, // From OpenGL 3.0 table 6.50.
    /*.maxVertexAttribs = */ 16,
    /*.maxVertexUniformComponents = */ 4096,
    /*.maxVaryingFloats = */ 60, // From OpenGLES 3.1 table 6.44.
    /*.maxVertexTextureImageUnits = */ 16,
    /*.maxCombinedTextureImageUnits = */ 80,
    /*.maxTextureImageUnits = */ 16,
    /*.maxFragmentUniformComponents = */ 1024,

    // glslang has 32 maxDrawBuffers.
    // Pixel phone Vulkan driver in Android N has 8
    // maxFragmentOutputAttachments.
    /*.maxDrawBuffers = */ 8,

    /*.maxVertexUniformVectors = */ 256,
    /*.maxVaryingVectors = */ 15, // From OpenGLES 3.1 table 6.44.
    /*.maxFragmentUniformVectors = */ 256,
    /*.maxVertexOutputVectors = */ 16,  // maxVertexOutputComponents / 4
    /*.maxFragmentInputVectors = */ 15, // maxFragmentInputComponents / 4
    /*.minProgramTexelOffset = */ -8,
    /*.maxProgramTexelOffset = */ 7,
    /*.maxClipDistances = */ 8,
    /*.maxComputeWorkGroupCountX = */ 65535,
    /*.maxComputeWorkGroupCountY = */ 65535,
    /*.maxComputeWorkGroupCountZ = */ 65535,
    /*.maxComputeWorkGroupSizeX = */ 1024,
    /*.maxComputeWorkGroupSizeX = */ 1024,
    /*.maxComputeWorkGroupSizeZ = */ 64,
    /*.maxComputeUniformComponents = */ 512,
    /*.maxComputeTextureImageUnits = */ 16,
    /*.maxComputeImageUniforms = */ 8,
    /*.maxComputeAtomicCounters = */ 8,
    /*.maxComputeAtomicCounterBuffers = */ 1, // From OpenGLES 3.1 Table 6.43
    /*.maxVaryingComponents = */ 60,
    /*.maxVertexOutputComponents = */ 64,
    /*.maxGeometryInputComponents = */ 64,
    /*.maxGeometryOutputComponents = */ 128,
    /*.maxFragmentInputComponents = */ 128,
    /*.maxImageUnits = */ 8, // This does not seem to be defined anywhere,
                             // set to ImageUnits.
    /*.maxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /*.maxCombinedShaderOutputResources = */ 8,
    /*.maxImageSamples = */ 0,
    /*.maxVertexImageUniforms = */ 0,
    /*.maxTessControlImageUniforms = */ 0,
    /*.maxTessEvaluationImageUniforms = */ 0,
    /*.maxGeometryImageUniforms = */ 0,
    /*.maxFragmentImageUniforms = */ 8,
    /*.maxCombinedImageUniforms = */ 8,
    /*.maxGeometryTextureImageUnits = */ 16,
    /*.maxGeometryOutputVertices = */ 256,
    /*.maxGeometryTotalOutputComponents = */ 1024,
    /*.maxGeometryUniformComponents = */ 512,
    /*.maxGeometryVaryingComponents = */ 60, // Does not seem to be defined
                                             // anywhere, set equal to
                                             // maxVaryingComponents.
    /*.maxTessControlInputComponents = */ 128,
    /*.maxTessControlOutputComponents = */ 128,
    /*.maxTessControlTextureImageUnits = */ 16,
    /*.maxTessControlUniformComponents = */ 1024,
    /*.maxTessControlTotalOutputComponents = */ 4096,
    /*.maxTessEvaluationInputComponents = */ 128,
    /*.maxTessEvaluationOutputComponents = */ 128,
    /*.maxTessEvaluationTextureImageUnits = */ 16,
    /*.maxTessEvaluationUniformComponents = */ 1024,
    /*.maxTessPatchComponents = */ 120,
    /*.maxPatchVertices = */ 32,
    /*.maxTessGenLevel = */ 64,
    /*.maxViewports = */ 16,
    /*.maxVertexAtomicCounters = */ 0,
    /*.maxTessControlAtomicCounters = */ 0,
    /*.maxTessEvaluationAtomicCounters = */ 0,
    /*.maxGeometryAtomicCounters = */ 0,
    /*.maxFragmentAtomicCounters = */ 8,
    /*.maxCombinedAtomicCounters = */ 8,
    /*.maxAtomicCounterBindings = */ 1,
    /*.maxVertexAtomicCounterBuffers = */ 0, // From OpenGLES 3.1 Table 6.41.

    // ARB_shader_atomic_counters.
    /*.maxTessControlAtomicCounterBuffers = */ 0,
    /*.maxTessEvaluationAtomicCounterBuffers = */ 0,
    /*.maxGeometryAtomicCounterBuffers = */ 0,
    // /ARB_shader_atomic_counters.

    /*.maxFragmentAtomicCounterBuffers = */ 0, // From OpenGLES 3.1 Table 6.43.
    /*.maxCombinedAtomicCounterBuffers = */ 1,
    /*.maxAtomicCounterBufferSize = */ 32,
    /*.maxTransformFeedbackBuffers = */ 4,
    /*.maxTransformFeedbackInterleavedComponents = */ 64,
    /*.maxCullDistances = */ 8,                // ARB_cull_distance.
    /*.maxCombinedClipAndCullDistances = */ 8, // ARB_cull_distance.
    /*.maxSamples = */ 4,
    /* .maxMeshOutputVerticesNV = */ 256,
    /* .maxMeshOutputPrimitivesNV = */ 512,
    /* .maxMeshWorkGroupSizeX_NV = */ 32,
    /* .maxMeshWorkGroupSizeY_NV = */ 1,
    /* .maxMeshWorkGroupSizeZ_NV = */ 1,
    /* .maxTaskWorkGroupSizeX_NV = */ 32,
    /* .maxTaskWorkGroupSizeY_NV = */ 1,
    /* .maxTaskWorkGroupSizeZ_NV = */ 1,
    /* .maxMeshViewCountNV = */ 4,
    /* .maxMeshOutputVerticesEXT = */ 256,
    /* .maxMeshOutputPrimitivesEXT = */ 256,
    /* .maxMeshWorkGroupSizeX_EXT = */ 128,
    /* .maxMeshWorkGroupSizeY_EXT = */ 128,
    /* .maxMeshWorkGroupSizeZ_EXT = */ 128,
    /* .maxTaskWorkGroupSizeX_EXT = */ 128,
    /* .maxTaskWorkGroupSizeY_EXT = */ 128,
    /* .maxTaskWorkGroupSizeZ_EXT = */ 128,
    /* .maxMeshViewCountEXT = */ 4,
    /* .maxDualSourceDrawBuffersEXT = */ 1,
    // This is the glslang TLimits structure.
    // It defines whether or not the following features are enabled.
    // We want them to all be enabled.
    /*.limits = */
    {
        /*.nonInductiveForLoops = */ 1,
        /*.whileLoops = */ 1,
        /*.doWhileLoops = */ 1,
        /*.generalUniformIndexing = */ 1,
        /*.generalAttributeMatrixVectorIndexing = */ 1,
        /*.generalVaryingIndexing = */ 1,
        /*.generalSamplerIndexing = */ 1,
        /*.generalVariableIndexing = */ 1,
        /*.generalConstantMatrixVectorIndexing = */ 1,
    }};

template <typename S0> static bool start_with(const S0& s, std::string const& prefix)
{
    if (s.size() < prefix.size()) {
        return false;
    } else if (s.size() == prefix.size()) {
        return s == prefix;
    } else {
        return s.substr(0, prefix.size()) == prefix;
    }
}

static std::map<std::string_view, int> stage_map{{"vertex", EShLangVertex},           {"vert", EShLangVertex},
                                                 {"fragment", EShLangFragment},       {"frag", EShLangFragment},
                                                 {"tesscontrol", EShLangTessControl}, {"tesc", EShLangTessControl},
                                                 {"tesseval", EShLangTessEvaluation}, {"tese", EShLangTessEvaluation},
                                                 {"geometry", EShLangGeometry},       {"geom", EShLangGeometryMask},
                                                 {"compute", EShLangCompute},         {"comp", EShLangCompute}};

EShLanguage map_to_stage(std::string_view const& name)
{

    // Valid stages are vertex, vert, fragment, frag, tesscontrol,
    // tesc, tesseval, tese, geometry, geom, compute, and comp.
    if (stage_map.count(name) <= 0) {
        return EShLangCount;
    }

    return (EShLanguage)stage_map[name];
}

static std::vector<EShLanguage> all_stages()
{
    std::vector<EShLanguage> stages;
    for (auto& [_, stage] : stage_map) {
        stages.push_back(static_cast<EShLanguage>(stage));
    }

    return stages;
}

static bool get_option_argument(const int argc, const char* argv[], int& i, const std::string& option,
                                std::string& value)
{
    std::string_view arg = argv[i];
    if (!start_with(arg, option)) {
        return false;
    }

    if (arg.size() != option.size()) {
        value = arg.substr(option.size());
        return true;
    }

    if (option.back() == '=') {
        value = "";
        return true;
    }

    if (i + 1 >= argc) {
        return false;
    }

    i += 1;
    value = argv[i];

    return true;
}

static bool get_stage_option_argument(const int argc, const char* argv[], int& i, std::string const& option,
                                      EShLanguage& stage, int& value)
{
    if (i + 1 >= argc) {
        return false;
    }

    stage = map_to_stage(argv[i + 1]);
    if (stage != EShLangCount) {
        i += 1;
        if (i + 1 >= argc) {
            return false;
        }
    }

    try {
        int offset = std::stoi(argv[i + 1]);
        value = offset;
    } catch (std::exception const& e) {
        std::cerr << "failed at get_stage_option_argument: " << e.what() << std::endl;
        return false;
    }

    i += 1;
    return true;
}

static bool get_resource_set_binding_argument(const int argc, const char* argv[], int& i, std::string const& option,
                                              EShLanguage& stage, std::vector<std::string>& regs,
                                              std::vector<int>& sets, std::vector<int>& bindings)
{
    if (i + 1 >= argc) {
        return false;
    }

    stage = map_to_stage(argv[i + 1]);
    if (stage != EShLangCount) {
        i += 1;
    }

    bool parse_triple = false;

    while (i + 3 < argc && argv[i + 1][0] != '-' && argv[i + 2][0] != '-' && argv[i + 3][0] != '-') {
        try {
            int set = std::stoi(argv[i + 2]);
            int binding = std::stoi(argv[i + 3]);
            std::string reg = argv[i + 1];
            sets.push_back(set);
            bindings.push_back(binding);
            regs.push_back(reg);
            parse_triple = true;
        } catch (...) {
            return false;
        }

        i += 3;
    }

    return parse_triple;
}

static bool set_resource_limit(std::string_view cmdline, CompileOption& option)
{
    std::string s(cmdline);
    std::istringstream iss(s);

    std::string resource;
    std::string value;
    iss >> resource >> value;

    try {
        auto ivalue = std::stoi(value);
#define RESOURCE_OP(field, name)                                                                                       \
    if (resource == #name) {                                                                                           \
        option.limits.field = ivalue;                                                                                  \
    }
#include "limits_file.inc"
#undef RESOURCE_OP
    } catch (...) {
        return false;
    }

    return true;
}

bool parse_compile_options(const int argc, const char* argv[], CompileOption& compile_option)
{
    for (int i = 1; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "--help" || arg == "-h") {
        } else if (arg == "--show-limits") {
        } else if (arg == "--version") {
        } else if (arg == "-o") {
        } else if (start_with(arg, "-fshader-stage=")) {
            auto const& stage = arg.substr(strlen("-fshader-stage="));
            compile_option.shader_stage = map_to_stage(stage);
        } else if (arg == "-fauto-bind-uniforms") {
            compile_option.auto_bind_uniforms = true;
        } else if (arg == "-fauto-combined-image-sampler") {
            compile_option.auto_combinded_image_sampler = true;
        } else if (arg == "-fauto-map-locations") {
            compile_option.auto_map_locations = true;
        } else if (arg == "-fhlsl-iomap") {
        } else if (arg == "-fhlsl-offsets") {

        } else if (arg == "-fhlsl_functionality1" || arg == "-fhlsl-functionality1") {
        } else if (arg == "-finvert-y") {
        } else if (arg == "-fnan-clamp") {
        } else if (arg == "-fpreserve-bindings") {
            compile_option.preserve_binddings = true;
        } else if (arg == "-fimage-binding-base") {
            EShLanguage stage = EShLangCount;
            int value = 0;
            if (get_stage_option_argument(argc, argv, i, "-fimage-binding-base", stage, value)) {
                compile_option.image_binding_base[stage] = value;
            }
        } else if (arg == "-ftexture-binding-base") {
            EShLanguage stage = EShLangCount;
            int value = 0;
            if (get_stage_option_argument(argc, argv, i, "-ftexture-binding-base", stage, value)) {
                compile_option.texture_binding_base[stage] = value;
            }
        } else if (arg == "-fsampler-binding-base") {
            EShLanguage stage = EShLangCount;
            int value = 0;
            if (get_stage_option_argument(argc, argv, i, "-fsampler-binding-base", stage, value)) {
                compile_option.sampler_binding_base[stage] = value;
            }
        } else if (arg == "-fubo-binding-base") {
            EShLanguage stage = EShLangCount;
            int value = 0;
            if (get_stage_option_argument(argc, argv, i, "-fubo-binding-base", stage, value)) {
                compile_option.ubo_binding_base[stage] = value;
            }
        } else if (arg == "-fcbuffer-binding-base") {
            EShLanguage stage = EShLangCount;
            int value = 0;
            if (get_stage_option_argument(argc, argv, i, "-fcbuffer-binding-base", stage, value)) {
                compile_option.ubo_binding_base[stage] = value;
            }
        } else if (arg == "-fssbo-binding-base") {
            EShLanguage stage = EShLangCount;
            int value = 0;
            if (get_stage_option_argument(argc, argv, i, "-fssbo-binding-base", stage, value)) {
                compile_option.ssbo_binding_base[stage] = value;
            }
        } else if (arg == "-fuav-binding-base") {
            EShLanguage stage = EShLangCount;
            int value = 0;
            if (get_stage_option_argument(argc, argv, i, "-fuav-binding-base", stage, value)) {
                compile_option.uav_binding_base[stage] = value;
            }
        } else if (arg == "-fresource-set-binding") {
            EShLanguage stage = EShLangCount;
            std::vector<std::string> regs;
            std::vector<int> sets, bindings;
            if (get_resource_set_binding_argument(argc, argv, i, "-fresource-set-binding", stage, regs, sets,
                                                  bindings)) {
                if (stage != EShLangCount) {
                    for (int k = 0; k < regs.size() && k < sets.size() && k < bindings.size(); ++k) {
                        const auto& name = regs[k];
                        const auto& set = sets[k];
                        const auto& binding = bindings[k];
                        compile_option.resource_set_binding[stage].push_back({name, set, binding});
                    }
                    continue;
                }

                for (auto const& stage : all_stages()) {
                    for (int k = 0; k < regs.size() && k < sets.size() && k < bindings.size(); ++k) {
                        const auto& name = regs[k];
                        const auto& set = sets[k];
                        const auto& binding = bindings[k];
                        compile_option.resource_set_binding[stage].push_back({name, set, binding});
                    }
                }
            }
        } else if (start_with(arg, "-fentry-point=")) {
            compile_option.entrypoint = arg.substr(strlen("-fentry-point="));
        } else if (start_with(arg, "-flimit=")) {
            std::string_view cmdline = arg.substr(strlen("-flimit="));
            set_resource_limit(cmdline, compile_option);
        } else if (arg == "-flimit-file") {
            std::string value;
            if (!get_option_argument(argc, argv, i, "-flimit-file", value)) {
                continue;
            }

            if (!std::filesystem::exists(value) || !std::filesystem::is_regular_file(value)) {
                continue;
            }

            std::ifstream ifs(value);
            std::string line;
            while (std::getline(ifs, line)) {
                set_resource_limit(line, compile_option);
            }
        } else if (start_with(arg, "-std=")) {
            auto version_profile = arg.substr(strlen("-std="));
            std::istringstream ss(std::string(version_profile.cbegin(), version_profile.cend()));
            int version = 0;
            std::string profile;
            if (!isdigit(version_profile.front())) {
                continue;
            }

            ss >> version >> profile;
            if (profile == "core") {
                compile_option.profile = ECoreProfile;
            } else if (profile == "es") {
                compile_option.profile = EEsProfile;
            } else {
                compile_option.profile = ENoProfile;
            }

            compile_option.version = version;
        } else if (start_with(arg, "--target-env=")) {
            auto target_env = arg.substr(strlen("--target-env="));
            if (target_env == "vulkan") {
                compile_option.client = glslang::EShClientVulkan;
                compile_option.client_version = glslang::EShTargetVulkan_1_0;
                compile_option.target_spv = glslang::EShTargetSpv_1_0;
            } else if (target_env == "vulkan1.0") {
                compile_option.client = glslang::EShClientVulkan;
                compile_option.client_version = glslang::EShTargetVulkan_1_0;
                compile_option.target_spv = glslang::EShTargetSpv_1_0;
            } else if (target_env == "vulkan1.1") {
                compile_option.client = glslang::EShClientVulkan;
                compile_option.client_version = glslang::EShTargetVulkan_1_1;
                compile_option.target_spv = glslang::EShTargetSpv_1_3;
            } else if (target_env == "vulkan1.2") {
                compile_option.client = glslang::EShClientVulkan;
                compile_option.client_version = glslang::EShTargetVulkan_1_2;
                compile_option.target_spv = glslang::EShTargetSpv_1_5;
            } else if (target_env == "vulkan1.3") {
                compile_option.client = glslang::EShClientVulkan;
                compile_option.client_version = glslang::EShTargetVulkan_1_3;
                compile_option.target_spv = glslang::EShTargetSpv_1_6;
            } else if (target_env == "vulkan1.4") {
                compile_option.client = glslang::EShClientVulkan;
                compile_option.client_version = glslang::EShTargetVulkan_1_4;
                compile_option.target_spv = glslang::EShTargetSpv_1_6;
            } else if (target_env == "opengl") {
                compile_option.client = glslang::EShClientOpenGL;
                compile_option.client_version = glslang::EShTargetOpenGL_450;
            } else if (target_env == "opengl4.5") {
                compile_option.client = glslang::EShClientOpenGL;
                compile_option.client_version = glslang::EShTargetOpenGL_450;
            }
        } else if (start_with(arg, "--target-spv=")) {
            auto target_spv = arg.substr(strlen("--target-spv"));

            if (target_spv == "spv1.0") {
                compile_option.target_spv = glslang::EShTargetSpv_1_0;
            } else if (target_spv == "spv1.1") {
                compile_option.target_spv = glslang::EShTargetSpv_1_1;
            } else if (target_spv == "spv1.2") {
                compile_option.target_spv = glslang::EShTargetSpv_1_2;
            } else if (target_spv == "spv1.3") {
                compile_option.target_spv = glslang::EShTargetSpv_1_3;
            } else if (target_spv == "spv1.4") {
                compile_option.target_spv = glslang::EShTargetSpv_1_4;
            } else if (target_spv == "spv1.5") {
                compile_option.target_spv = glslang::EShTargetSpv_1_5;
            } else if (target_spv == "spv1.6") {
                compile_option.target_spv = glslang::EShTargetSpv_1_6;
            }
        } else if (start_with(arg, "-mfmt=")) {
            auto mfmt = arg.substr(strlen("-mfmt="));
            if (mfmt == "bin") {

            } else if (mfmt == "c") {

            } else if (mfmt == "num") {
            }
        } else if (arg == "-x") {
            std::string value;
            //support glsl only
            if (!get_option_argument(argc, argv, i, "-x", value)) {
                continue;
            }

            if (value == "glsl") {
                compile_option.language = glslang::EShSourceGlsl;
            } else if (value == "hlsl") {
                compile_option.language = glslang::EShSourceHlsl;
            }
        } else if (arg == "-c") {
            continue;
        } else if (arg == "-E") {
            continue;
        } else if (arg == "-M" || arg == "-MM") {
            continue;
        } else if (arg == "-MD") {
            continue;
        } else if (arg == "-MT") {
            std::string value;
            get_option_argument(argc, argv, i, "-MT", value);
            continue;
        } else if (arg == "-MF") {
            std::string value;
            get_option_argument(argc, argv, i, "-MF", value);
        } else if (arg == "-S") {
            continue;
        } else if (start_with(arg, "-D")) {
            if (arg.size() <= 2) {
                continue;
            }

            const auto macro = arg.substr(2);
            const auto pos = macro.find_first_of('=');
            if (pos == std::string_view::npos) {
                std::string name(macro);
                compile_option.macros[name] = "";
                continue;
            }

            const std::string name(macro.substr(0, pos));
            std::string value;
            if (pos == macro.size() - 1) {
                value = "";
            } else {
                value = macro.substr(pos + 1);
            }
            compile_option.macros[name] = value;
        } else if (start_with(arg, "-I")) {
            std::string value;
            if (get_option_argument(argc, argv, i, "-I", value)) {
                compile_option.include_dirs.push_back(value);
            }
        } else if (arg == "-g") {
            continue;
        } else if (start_with(arg, "-O")) {
            continue;
        } else if (arg == "-w") {
            compile_option.suppress_warining = true;
        } else if (arg == "-Werror") {
            compile_option.warnings_as_errors = true;
        } else if (!(arg == "-") && arg[0] == '-') {
            continue;
        } else {
            if (arg == "-") {
                continue;
            }

            compile_option.filename = arg;
        }
    }
    return true;
}

bool parse_compile_options(std::vector<std::string> const& args, CompileOption& compile_options)
{
    std::vector<const char*> argv;
    for (const auto& arg : args) {
        argv.push_back(arg.c_str());
    }

    return parse_compile_options(argv.size(), argv.data(), compile_options);
}
