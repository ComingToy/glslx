#include "parser.hpp"
#include "glslang/MachineIndependent/Initialize.h"
#include <memory>
static glslang::TParseContext* CreateParseContext(glslang::TSymbolTable& symbolTable,
                                                  glslang::TIntermediate& intermediate, int version, EProfile profile,
                                                  glslang::EShSource source, EShLanguage language, TInfoSink& infoSink,
                                                  glslang::SpvVersion spvVersion, bool forwardCompatible,
                                                  EShMessages messages, bool parsingBuiltIns,
                                                  std::string sourceEntryPointName = "")
{
    switch (source) {
    case glslang::EShSourceGlsl: {
        if (sourceEntryPointName.size() == 0)
            intermediate.setEntryPointName("main");
        glslang::TString entryPoint = sourceEntryPointName.c_str();
        return new glslang::TParseContext(symbolTable, intermediate, parsingBuiltIns, version, profile, spvVersion,
                                          language, infoSink, forwardCompatible, messages, &entryPoint);
    }

    default:
        infoSink.info.message(glslang::EPrefixInternalError, "Unable to determine source language");
        return nullptr;
    }
}

static bool InitializeSymbolTable(const glslang::TString& builtIns, int version, EProfile profile,
                                  const glslang::SpvVersion& spvVersion, EShLanguage language,
                                  glslang::EShSource source, TInfoSink& infoSink, glslang::TSymbolTable& symbolTable)
{
    glslang::TIntermediate intermediate(language, version, profile);

    intermediate.setSource(source);

    std::unique_ptr<glslang::TParseContextBase> parseContext(CreateParseContext(symbolTable, intermediate, version,
                                                                                profile, source, language, infoSink,
                                                                                spvVersion, true, EShMsgDefault, true));

    glslang::TShader::ForbidIncluder includer;
    glslang::TPpContext ppContext(*parseContext, "", includer);
    glslang::TScanContext scanContext(*parseContext);
    parseContext->setScanContext(&scanContext);
    parseContext->setPpContext(&ppContext);

    //
    // Push the symbol table to give it an initial scope.  This
    // push should not have a corresponding pop, so that built-ins
    // are preserved, and the test for an empty table fails.
    //

    symbolTable.push();

    const char* builtInShaders[2];
    size_t builtInLengths[2];
    builtInShaders[0] = builtIns.c_str();
    builtInLengths[0] = builtIns.size();

    if (builtInLengths[0] == 0)
        return true;

    glslang::TInputScanner input(1, builtInShaders, builtInLengths);
    if (!parseContext->parseShaderStrings(ppContext, input) != 0) {
        infoSink.info.message(glslang::EPrefixInternalError, "Unable to parse built-ins");
        fprintf(stderr, "Unable to parse built-ins\n%s\n", infoSink.info.c_str());
        fprintf(stderr, "%s\n", builtInShaders[0]);

        return false;
    }

    return true;
}

static bool AddContextSpecificSymbols(const TBuiltInResource* resources, TInfoSink& infoSink,
                                      glslang::TSymbolTable& symbolTable, int version, EProfile profile,
                                      const glslang::SpvVersion& spvVersion, EShLanguage language,
                                      glslang::EShSource source)
{
    std::unique_ptr<glslang::TBuiltInParseables> builtInParseables(new glslang::TBuiltIns());

    if (builtInParseables == nullptr)
        return false;

    builtInParseables->initialize(*resources, version, profile, spvVersion, language);
    if (!InitializeSymbolTable(builtInParseables->getCommonString(), version, profile, spvVersion, language, source,
                               infoSink, symbolTable))
        return false;
    builtInParseables->identifyBuiltIns(version, profile, spvVersion, language, symbolTable, *resources);

    return true;
}

std::unique_ptr<ParserResouce> create_parser(const int version, EProfile profile, EShLanguage stage,
                                             glslang::SpvVersion spvVersion, const char* entrypoint)
{
    static const TBuiltInResource kDefaultTBuiltInResource = {
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

    glslang::TSymbolTable* symbolTable(new glslang::TSymbolTable);
    TInfoSink infoSink;
    AddContextSpecificSymbols(&kDefaultTBuiltInResource, infoSink, *symbolTable, version, profile, spvVersion, stage,
                              glslang::EShSourceGlsl);

    auto intermediate = new glslang::TIntermediate(stage);
    const EShMessages message = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgSpvRules | EShMsgVulkanRules);

    glslang::TParseContext* parseContext(CreateParseContext(*symbolTable, *intermediate, version, profile,
                                                            glslang::EShSourceGlsl, stage, infoSink, spvVersion, false,
                                                            message, false, entrypoint));
    parseContext->compileOnly = false;

    glslang::TScanContext* scanContext(new glslang::TScanContext(*parseContext));
    parseContext->setScanContext(scanContext);

    auto includer = new DirStackFileIncluder();

    auto* ppContext(new glslang::TPpContext(*parseContext, "", *includer));
    parseContext->setPpContext(ppContext);

    auto resource = new ParserResouce(symbolTable, intermediate, parseContext, scanContext, ppContext, includer);

    return std::unique_ptr<ParserResouce>(resource);
}
