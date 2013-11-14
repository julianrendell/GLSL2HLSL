// AngleTranslatorDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "AngleTranslatorDLL.h"

ANGLETRANSLATORDLL_API int ShaderInitialize()
{
	return ShInitialize();
}

ANGLETRANSLATORDLL_API ShHandle ShaderConstructCompiler(ShShaderType type,
    ShShaderSpec spec,
    ShShaderOutput output,
    const ShBuiltInResources* resources)
{
	return ShConstructCompiler(type, spec, output, resources);
}

ANGLETRANSLATORDLL_API int ShaderCompile(const ShHandle handle,
    const char* const shaderStrings[],
    size_t numStrings,
    int compileOptions
    )
{
	return ShCompile(handle, shaderStrings, numStrings, compileOptions);
}

ANGLETRANSLATORDLL_API void ShaderDestruct(ShHandle handle)
{
	ShDestruct(handle);
}

ANGLETRANSLATORDLL_API int ShaderFinalize()
{
	return ShFinalize();
}

ANGLETRANSLATORDLL_API void ShaderInitBuiltInResources(ShBuiltInResources* resources)
{
	ShInitBuiltInResources(resources);
}

ANGLETRANSLATORDLL_API void ShaderGetInfo(const ShHandle handle, ShShaderInfo pname, size_t* params)
{
	ShGetInfo(handle, pname, params);
}

ANGLETRANSLATORDLL_API VOID ShaderGetObjectCode(const ShHandle handle, char* objCode)
{
	ShGetObjectCode(handle, objCode);
}