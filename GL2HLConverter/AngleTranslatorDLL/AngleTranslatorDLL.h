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

#ifndef ANGLETRANSLATORDLL_H
#define ANGLETRANSLATORDLL_H

#include <stdio.h>
#include <gl\GL.h>
#include "GLSLANG\ShaderLang.h"
#include "GLES2\gl2.h"
#include "libGLESv2\Shader.h"
#include "libGLESv2\Program.h"
#include "libGLESv2\Context.h"
#include "libGLESv2\main.h"
#include "libGLESv2\Program.h"
//#include "libGLESv2\ResourceManager.h"

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

ANGLETRANSLATORDLL_API void ParseAllAttributes(const char *hlsl);

ANGLETRANSLATORDLL_API void ParseAllVaryings();

ANGLETRANSLATORDLL_API GLuint LoadShader(GLenum type, const char *shaderSrc);

ANGLETRANSLATORDLL_API GLuint GetHLSL(const char *shaderSrc);

ANGLETRANSLATORDLL_API bool linkVaryings(gl::InfoLog &infoLog, int registers, const gl::Varying *packing[][4],
                      std::string& pixelHLSL, std::string& vertexHLSL,
                      gl::FragmentShader *fragmentShader, gl::VertexShader *vertexShader);

ANGLETRANSLATORDLL_API bool link(gl::InfoLog &infoLog, const gl::AttributeBindings &attributeBindings, gl::FragmentShader *fragmentShader, gl::VertexShader *vertexShader);

#endif