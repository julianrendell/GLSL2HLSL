// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the ANGLETRANSLATORDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// ANGLETRANSLATORDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef ANGLETRANSLATORDLL_EXPORTS
#define ANGLETRANSLATORDLL_API __declspec(dllexport)
#else
#define ANGLETRANSLATORDLL_API __declspec(dllimport)
#endif

#include "GLSLANG\ShaderLang.h"

ANGLETRANSLATORDLL_API int ShaderInitialize();

ANGLETRANSLATORDLL_API ShHandle ShaderConstructCompiler(ShShaderType type,
    ShShaderSpec spec,
    ShShaderOutput output,
    const ShBuiltInResources* resources);

ANGLETRANSLATORDLL_API int ShaderCompile(const ShHandle handle,
    const char* const shaderStrings[],
    size_t numStrings,
    int compileOptions
    );

ANGLETRANSLATORDLL_API void ShaderDestruct(ShHandle handle);

ANGLETRANSLATORDLL_API int ShaderFinalize();

ANGLETRANSLATORDLL_API void ShaderInitBuiltInResources(ShBuiltInResources* resources);

ANGLETRANSLATORDLL_API void ShaderGetInfo(const ShHandle handle, ShShaderInfo pname, size_t* params);

ANGLETRANSLATORDLL_API void ShaderGetObjectCode(const ShHandle handle, char* objCode);