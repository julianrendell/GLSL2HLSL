#include "Monolith.h"

void LinkUniforms()
{

}

void LinkAttributes()
{

}

void LinkVaryings()
{

}

bool Link()
{
	// Map the varyings to the register file
    const Varying *packing[IMPLEMENTATION_MAX_VARYING_VECTORS][4] = {NULL};
    int registers = packVaryings(infoLog, packing, fragmentShader);

	return false;
}

void BindAttributeLocation(GLuint index, const GLchar* name)
{
	if (index >= MAX_VERTEX_ATTRIBS)
    {
		throw new std::runtime_error("Unable to bind attribute locations. Index exceeds max vertex attribute count.");
    }

	if (strncmp(name, "gl_", 3) == 0)
    {
		throw new std::runtime_error("The attribute name does not start with 'gl_'");
    }

	if (index < MAX_VERTEX_ATTRIBS)
    {
        for (int i = 0; i < MAX_VERTEX_ATTRIBS; i++)
        {
            attributeBinding[i].erase(name);
        }

        attributeBinding[index].insert(name);
    }
}

// This is taken from Shader.cpp
// true if varying x has a higher priority in packing than y
bool CompareVarying(const Varying &x, const Varying &y)
{
    if(x.type == y.type)
    {
        return x.size > y.size;
    }

    switch (x.type)
    {
      case GL_FLOAT_MAT4: return true;
      case GL_FLOAT_MAT2:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return true;
          case GL_FLOAT_VEC4: return true;
          case GL_FLOAT_MAT3: return true;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_VEC4:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return true;
          case GL_FLOAT_MAT3: return true;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_MAT3:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return false;
          case GL_FLOAT_MAT3: return true;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_VEC3:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return false;
          case GL_FLOAT_MAT3: return false;
          case GL_FLOAT_VEC3: return true;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT_VEC2:
        switch(y.type)
        {
          case GL_FLOAT_MAT4: return false;
          case GL_FLOAT_MAT2: return false;
          case GL_FLOAT_VEC4: return false;
          case GL_FLOAT_MAT3: return false;
          case GL_FLOAT_VEC3: return false;
          case GL_FLOAT_VEC2: return true;
          case GL_FLOAT:      return true;
          default: return false;
        }
        break;
      case GL_FLOAT: return false;
      default: return false;
    }

    return false;
}

bool IsCompiled(char * hlsl)
{
	return hlsl != NULL;
}

GLenum ParseType(const std::string &type)
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
    else
	{
		return GL_NONE;
	}
}

void ParseAttributes(char * hlsl)
{
    if (hlsl)
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

            attributes.push_back(Attribute(ParseType(attributeType), attributeName));

            input = strstr(input, ";") + 2;
        }
    }
}

void ParseVaryings(char * hlsl)
{
	if (hlsl)
    {
        const char *input = strstr(vertexHLSL, "// Varyings") + 12;

        while(true)
        {
            char varyingType[256];
            char varyingName[256];

            int matches = sscanf(input, "static %255s %255s", varyingType, varyingName);

            if (matches != 2)
            {
                break;
            }

            char *array = strstr(varyingName, "[");
            int size = 1;

            if (array)
            {
                size = atoi(array + 1);
                *array = '\0';
            }

			varyings.push_back(Varying(ParseType(varyingType), varyingName, size, array != NULL));

            input = strstr(input, ";") + 2;
        }

        usesMultipleRenderTargets = strstr(hlsl, "GL_USES_MRT") != NULL;
        usesFragColor = strstr(hlsl, "GL_USES_FRAG_COLOR") != NULL;
        usesFragData = strstr(hlsl, "GL_USES_FRAG_DATA") != NULL;
        usesFragCoord = strstr(hlsl, "GL_USES_FRAG_COORD") != NULL;
        usesFrontFacing = strstr(hlsl, "GL_USES_FRONT_FACING") != NULL;
        usesPointSize = strstr(hlsl, "GL_USES_POINT_SIZE") != NULL;
        usesPointCoord = strstr(hlsl, "GL_USES_POINT_COORD") != NULL;
        usesDepthRange = strstr(hlsl, "GL_USES_DEPTH_RANGE") != NULL;
        usesFragDepth = strstr(hlsl, "GL_USES_FRAG_DEPTH") != NULL;
        usesDiscardRewriting = strstr(hlsl, "ANGLE_USES_DISCARD_REWRITING") != NULL;
    }
}

void CompileToHLSL(void * compiler, const char * shaderSrc, char * hlsl, char * infoLog)
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

void CompileVertexShader(const char * vertexShaderSrc)
{
	CompileToHLSL(vertexCompiler, vertexShaderSrc, vertexHLSL, vertexInfoLog);

	if (!IsCompiled(vertexHLSL))
	{
		throw new std::runtime_error("Vertex shader compilation has failed! (HLSL string is empty)");
	}

	ParseAttributes(vertexHLSL);
	ParseVaryings(vertexHLSL);
}

void CompileFragmentShader(const char * fragmentShaderSrc)
{
	CompileToHLSL(fragmentCompiler, fragmentShaderSrc, fragmentHLSL, fragmentInfoLog);

	if (!IsCompiled(fragmentHLSL))
	{
		throw new std::runtime_error("Fragment shader compilation has failed! (HLSL string is empty)");
	}

	ParseVaryings(fragmentHLSL);
	varyings.sort(CompareVarying);
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

void Humongoid(const char * vertexShaderSrc, const char * fragmentShaderSrc)
{
	ShBuiltInResources resources = InitBuiltInResources();
	ConstructCompiler(resources);

	CompileVertexShader(vertexShaderSrc);
	CompileFragmentShader(fragmentShaderSrc);
	
	BindAttributeLocation(0, "vPosition");

	Link();
};




