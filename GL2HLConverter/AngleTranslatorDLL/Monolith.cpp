#include "Monolith.h"

// Refactoring incomplete!
void CompileShader(void * compiler, const char * shaderSrc)
{
	int result = ShCompile(compiler, &shaderSrc, 1, SH_OBJECT_CODE);

	if (result)
    {
        size_t objCodeLen = 0;
        ShGetInfo(compiler, SH_OBJECT_CODE_LENGTH, &objCodeLen);
        hlsl = new char[objCodeLen];
        ShGetObjectCode(compiler, hlsl);

        void *activeUniforms;
        ShGetInfoPointer(compiler, SH_ACTIVE_UNIFORMS_ARRAY, &activeUniforms);
        activeUniforms = (ActiveUniforms*)activeUniforms;
    }
    else
    {
        size_t infoLogLen = 0;
        ShGetInfo(compiler, SH_INFO_LOG_LENGTH, &infoLogLen);
        infoLog = new char[infoLogLen];
        ShGetInfoLog(compiler, infoLog);

		throw new std::runtime_error(infoLog);
    }
}

void ConstructCompiler(ShBuiltInResources resources)
{
	fragmentCompiler = ShConstructCompiler(SH_FRAGMENT_SHADER, SH_GLES2_SPEC, SH_HLSL11_OUTPUT, &resources);
	if (NULL == fragmentCompiler)
	{
		throw new std::runtime_error("Fragment compiler construction failed!");
	}

	vertexCompiler = ShConstructCompiler(SH_VERTEX_SHADER, SH_GLES2_SPEC, SH_HLSL11_OUTPUT, &resources);
	if (NULL == vertexCompiler)
	{
		throw new std::runtime_error("Vertex compiler construction failed!");
	}
}

ShBuiltInResources InitBuiltInResources()
{
	ShBuiltInResources resources;

	ShInitBuiltInResources(&resources);

	// These are values specific to DX11, from Renderer11.cpp
    resources.MaxVertexAttribs = 16;
    resources.MaxVertexUniformVectors = 1024;
    resources.MaxVaryingVectors = 32;
    resources.MaxVertexTextureImageUnits = 16;
    resources.MaxCombinedTextureImageUnits = 16 + 16;
    resources.MaxTextureImageUnits = 16;
    resources.MaxFragmentUniformVectors = 1024;
    resources.MaxDrawBuffers = 8;

	resources.OES_standard_derivatives = true;
	resources.EXT_draw_buffers = 8 > 1;
	resources.FragmentPrecisionHigh = 1;
	resources.EXT_frag_depth = 1;

	return resources;
}

void Humongoid(const char * fragmentShaderSrc, const char * vertexShaderSrc)
{
	ShBuiltInResources resources = InitBuiltInResources();

	ConstructCompiler(resources);
	CompileShader(fragmentCompiler, fragmentShaderSrc);
	CompileShader(vertexCompiler, vertexShaderSrc);
};




