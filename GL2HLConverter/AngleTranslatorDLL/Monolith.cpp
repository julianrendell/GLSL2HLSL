#include "Monolith.h"

void linkUniforms()
{

}

void linkAttributes()
{

}

void linkVaryings()
{

}

bool link()
{
	return false;
}

void bindAttributeLocation(GLuint index, const GLchar* name)
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
bool compareVarying(const Varying &x, const Varying &y)
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
    else
	{
		return GL_NONE;
	}
}

void VertexShader::parseAttributes()
{
	if (mHlsl)
    {
		const char *input = strstr(mHlsl, "// Attributes") + 14;

        while(true)
        {
            char attributeType[256];
            char attributeName[256];

            int matches = sscanf(input, "static %255s _%255s", attributeType, attributeName);

            if (matches != 2)
            {
                break;
            }

			mAttributes.push_back(Attribute(parseType(attributeType), attributeName));

            input = strstr(input, ";") + 2;
        }
    }
}

void Shader::parseVaryings()
{
	if (mHlsl)
    {
		const char *input = strstr(mHlsl, "// Varyings") + 12;

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

			mVaryings.push_back(Varying(parseType(varyingType), varyingName, size, array != NULL));

            input = strstr(input, ";") + 2;
        }

		mUsesMultipleRenderTargets = strstr(mHlsl, "GL_USES_MRT") != NULL;
        mUsesFragColor = strstr(mHlsl, "GL_USES_FRAG_COLOR") != NULL;
        mUsesFragData = strstr(mHlsl, "GL_USES_FRAG_DATA") != NULL;
        mUsesFragCoord = strstr(mHlsl, "GL_USES_FRAG_COORD") != NULL;
        mUsesFrontFacing = strstr(mHlsl, "GL_USES_FRONT_FACING") != NULL;
        mUsesPointSize = strstr(mHlsl, "GL_USES_POINT_SIZE") != NULL;
        mUsesPointCoord = strstr(mHlsl, "GL_USES_POINT_COORD") != NULL;
        mUsesDepthRange = strstr(mHlsl, "GL_USES_DEPTH_RANGE") != NULL;
        mUsesFragDepth = strstr(mHlsl, "GL_USES_FRAG_DEPTH") != NULL;
        mUsesDiscardRewriting = strstr(mHlsl, "ANGLE_USES_DISCARD_REWRITING") != NULL;
    }
}

void compileToHLSL(Shader *shader)
{
	int result = ShCompile(shader->compiler, &shader->mSource, 1, SH_OBJECT_CODE);

	if (result)
    {
        size_t objCodeLen = 0;
		ShGetInfo(shader->compiler, SH_OBJECT_CODE_LENGTH, &objCodeLen);
		shader->mHlsl = new char[objCodeLen];
		ShGetObjectCode(shader->compiler, shader->mHlsl);

        void *activeUniforms;
		ShGetInfoPointer(shader->compiler, SH_ACTIVE_UNIFORMS_ARRAY, &activeUniforms);
        activeUniforms = (ActiveUniforms*)activeUniforms;
    }
    else
    {
        size_t infoLogLen = 0;
		ShGetInfo(shader->compiler, SH_INFO_LOG_LENGTH, &infoLogLen);
		shader->mInfoLog = new char[infoLogLen];
		ShGetInfoLog(shader->compiler, shader->mInfoLog);

		throw new std::runtime_error(shader->mInfoLog);
    }
}

void compileVertexShader(VertexShader *vShader)
{
	compileToHLSL(vShader);

	if (!IsCompiled(vShader->mHlsl))
	{
		throw new std::runtime_error("Vertex shader compilation has failed! (HLSL string is empty)");
	}

	vShader->parseAttributes();
	vShader->parseVaryings();
}

void compileFragmentShader(FragmentShader *fShader)
{
	compileToHLSL(fShader);

	if (!IsCompiled(fShader->mHlsl))
	{
		throw new std::runtime_error("Fragment shader compilation has failed! (HLSL string is empty)");
	}

	fShader->parseVaryings();
	fShader->mVaryings.sort(compareVarying);
}

void constructCompiler(ShBuiltInResources resources)
{
	fShader->compiler = ShConstructCompiler(SH_FRAGMENT_SHADER, SH_GLES2_SPEC, SH_HLSL11_OUTPUT, &resources);
	if (NULL == fShader->compiler)
	{
		throw new std::runtime_error("Fragment compiler construction failed!");
	}

	vShader->compiler = ShConstructCompiler(SH_VERTEX_SHADER, SH_GLES2_SPEC, SH_HLSL11_OUTPUT, &resources);
	if (NULL == vShader->compiler)
	{
		throw new std::runtime_error("Vertex compiler construction failed!");
	}
}

ShBuiltInResources initBuiltInResources()
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

void humongoid(const char * vertexShaderSrc, const char * fragmentShaderSrc)
{
	vShader = new VertexShader();
	vShader->mSource = (char*) vertexShaderSrc;

	fShader = new FragmentShader();
	fShader->mSource = (char*) fragmentShaderSrc;

	ShBuiltInResources resources = initBuiltInResources();
	constructCompiler(resources);

	compileVertexShader(vShader);
	compileFragmentShader(fShader);
	
	bindAttributeLocation(0, "vPosition");

	link();
};




