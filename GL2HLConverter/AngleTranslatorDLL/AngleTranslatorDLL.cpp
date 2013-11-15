// AngleTranslatorDLL.cpp : Defines the exported functions for the DLL application.
//

//#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "stdio.h"
#include "AngleTranslatorDLL.h"
#include "compiler\debug.h"

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

ANGLETRANSLATORDLL_API void ShaderGetObjectCode(const ShHandle handle, char* objCode)
{
	ShGetObjectCode(handle, objCode);
}


GLenum parseType(const std::string &type)
{
    if (type == "float")
    {
        return GL_FLOAT;
    }
    else if (type == "float2")
    {
        return GL_FLOAT_VEC2;
    }
    else if (type == "float3")
    {
        return GL_FLOAT_VEC3;
    }
    else if (type == "float4")
    {
        return GL_FLOAT_VEC4;
    }
    else if (type == "float2x2")
    {
        return GL_FLOAT_MAT2;
    }
    else if (type == "float3x3")
    {
        return GL_FLOAT_MAT3;
    }
    else if (type == "float4x4")
    {
        return GL_FLOAT_MAT4;
    }
    else UNREACHABLE();

    return GL_NONE;
}

ANGLETRANSLATORDLL_API void ParseAllAttributes(const char *hlsl)
{
   /* if (hlsl)
    {
        const char *input = strstr(hlsl, "// Attributes") + 14;

        while(true)
        {
            char attributeType[256];
            char attributeName[256];

            int matches = sscanf(input, "static %255s _%255s", attributeType, attributeName);

            if (matches != 2)
            {
                break;
            }

            mAttributes.push_back(gl::Attribute(parseType(attributeType), attributeName));

            input = strstr(input, ";") + 2;
        }
    }*/
}